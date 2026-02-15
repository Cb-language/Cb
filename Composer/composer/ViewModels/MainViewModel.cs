using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text.Json;
using System.Threading.Tasks;
using System.Timers; 
using Avalonia.Platform.Storage;
using Avalonia.Threading; 
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using composer.ANSIHelper;

namespace composer.ViewModels;

public class AppSettings
{
    public string? TranspilerPath { get; set; }
}

public partial class MainViewModel : ViewModelBase
{
    public IStorageProvider? StorageProvider { get; set; }
    
    private readonly string _settingsFilePath = Path.Combine(AppContext.BaseDirectory, "composer.settings.json");

    // LSP Debounce Timer
    private readonly Timer _debounceTimer;
    private bool _isLspRunning = false;

    [ObservableProperty] private bool _expanded = true;
    [ObservableProperty] private double _borderWidth = 250;
    
    [ObservableProperty] private string _transpilerExePath = "Not Set";

    [ObservableProperty] 
    private FileTabViewModel? _selectedTab;

    [ObservableProperty] private string? _mainFilePath;
    [ObservableProperty] private string? _currentFolderPath;

    public ObservableCollection<FileTabViewModel> Files { get; } = new();
    
    public ObservableCollection<string> FolderFiles { get; } = new();
    
    public AnsiTerminal Terminal { get; } = new();
    
    public MainViewModel()
    {
        LoadSettings();
        
        // Initialize Debounce Timer (500ms delay)
        _debounceTimer = new Timer(500);
        _debounceTimer.AutoReset = false; 
        _debounceTimer.Elapsed += async (s, e) => 
        {
            await Dispatcher.UIThread.InvokeAsync(async () => await RunLSP());
        };

        AddNewTab();
    }

    protected override void OnPropertyChanged(System.ComponentModel.PropertyChangedEventArgs e)
    {
        base.OnPropertyChanged(e);

        if (e.PropertyName == nameof(SelectedTab))
        {
            if (SelectedTab != null)
            {
                SelectedTab.PropertyChanged -= OnTabPropertyChanged; 
                SelectedTab.PropertyChanged += OnTabPropertyChanged;
                
                // Check immediately on tab switch
                _debounceTimer.Stop();
                _debounceTimer.Start();
            }
        }
    }

    private void OnTabPropertyChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
    {
        if (e.PropertyName == nameof(FileTabViewModel.Content))
        {
            _debounceTimer.Stop();
            _debounceTimer.Start();
        }
    }

    partial void OnTranspilerExePathChanged(string value)
    {
        SaveSettings();
    }

    private void LoadSettings()
    {
        if (!File.Exists(_settingsFilePath)) return;
        try
        {
            var json = File.ReadAllText(_settingsFilePath);
            var settings = JsonSerializer.Deserialize<AppSettings>(json);

            if (settings != null && !string.IsNullOrEmpty(settings.TranspilerPath))
            {
                TranspilerExePath = settings.TranspilerPath;
            }
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Error loading settings: {ex.Message}");
        }
    }

    private void SaveSettings()
    {
        try
        {
            var settings = new AppSettings { TranspilerPath = TranspilerExePath };
            var json = JsonSerializer.Serialize(settings);
            File.WriteAllText(_settingsFilePath, json);
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Error saving settings: {ex.Message}");
        }
    }
    partial void OnExpandedChanged(bool value) => BorderWidth = value ? 250 : 70;
    
    [RelayCommand]
    private void SideMenuResize() => Expanded = !Expanded;

    [RelayCommand]
    private void AddNewTab()
    {
        AddTab($"Untitled {Files.Count + 1}", "");
    }

    [RelayCommand]
    private void CloseCurrentTab()
    {
        if (SelectedTab != null)
        {
            CloseTab(SelectedTab);
        }
    }

    [RelayCommand]
    private void NextTab()
    {
        if (Files.Count < 2 || SelectedTab == null) return;
        var index = Files.IndexOf(SelectedTab);
        var nextIndex = (index + 1) % Files.Count;
        SelectedTab = Files[nextIndex];
    }

    [RelayCommand]
    private async Task OpenFolder()
    {
        if (StorageProvider == null) return;
        var result = await StorageProvider.OpenFolderPickerAsync(new FolderPickerOpenOptions
        {
            Title = "Open Folder",
            AllowMultiple = false
        });

        if (result.Count == 1)
        {
            CurrentFolderPath = result[0].Path.LocalPath;
            FolderFiles.Clear();
            var files = Directory.GetFiles(CurrentFolderPath, "*.cb", SearchOption.TopDirectoryOnly);
            foreach (var file in files)
            {
                FolderFiles.Add(Path.GetFileName(file));
            }
        }
        if (CurrentFolderPath != null) FindPreludeFile(CurrentFolderPath);
    }

    private string? FindPreludeFile(string folderPath)
    {
        var found = 0;
        string? mainFile = null;
        var files = Directory.GetFiles(folderPath, "*.cb", SearchOption.TopDirectoryOnly);
        foreach (var file in files)
        {
            try
            {
                var content = File.ReadAllText(file);
                if (!content.Contains("prelude", StringComparison.OrdinalIgnoreCase)) continue;
                found++;
                mainFile = file;
            }
            catch (Exception ex) { Debug.WriteLine($"Error reading file {file}: {ex.Message}"); }
        }

        if (found == 1)
        {
            MainFilePath = mainFile;
            return mainFile;
        }
        MainFilePath = null;
        return null;
    }

    [RelayCommand]
    private async Task OpenSourceFile()
    {
        if (StorageProvider == null) return;
        var result = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Open Source File",
            AllowMultiple = false,
            FileTypeFilter = [new FilePickerFileType("Composer Files") { Patterns = ["*.cb"] }]
        });

        if (result.Count == 1)
        {
            var file = result[0];
            var path = file.Path.LocalPath;
            await using var stream = await file.OpenReadAsync();
            using var reader = new StreamReader(stream);
            var text = await reader.ReadToEndAsync();

            if (SelectedTab == null || (SelectedTab.Header.StartsWith("Untitled") && string.IsNullOrEmpty(SelectedTab.Content)))
            {
                if (SelectedTab == null) AddNewTab();
            }
            else
            {
                AddNewTab();
            }

            if (SelectedTab != null)
            {
                SelectedTab.Header = file.Name;
                SelectedTab.FilePath = path;
                SelectedTab.Content = text;
                SelectedTab.IsModified = false;
            }
        }
    }

    [RelayCommand]
    private async Task SaveSource()
    {
        if (SelectedTab == null || StorageProvider == null) return;

        if (string.IsNullOrEmpty(SelectedTab.FilePath))
        {
            var file = await StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
            {
                Title = "Save Source File",
                DefaultExtension = "cb",
                FileTypeChoices = [new FilePickerFileType("Composer File") { Patterns = ["*.cb"] }]
            });

            if (file != null)
                SelectedTab.FilePath = file.Path.LocalPath;
            else
                return;
        }

        try 
        {
            await File.WriteAllTextAsync(SelectedTab.FilePath!, SelectedTab.Content);
            SelectedTab.IsModified = false;
            
            // --- FIX: Run LSP Immediately on Save ---
            await RunLSP();
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Error saving file: {ex.Message}");
        }
        
        if(CurrentFolderPath != null)
            FindPreludeFile(CurrentFolderPath);
    }

    private async Task<bool> SaveTab(FileTabViewModel tab)
    {
        if (StorageProvider == null) return false;
        if (string.IsNullOrEmpty(tab.FilePath))
        {
            var file = await StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
            {
                Title = $"Save Source File: {tab.Header}",
                DefaultExtension = "cb",
                FileTypeChoices = [new FilePickerFileType("Composer File") { Patterns = ["*.cb"] }]
            });

            if (file != null)
            {
                tab.FilePath = file.Path.LocalPath;
                tab.Header = file.Name;
            }
            else return false;
        }

        try
        {
            await File.WriteAllTextAsync(tab.FilePath!, tab.Content);
            tab.IsModified = false;
        }
        catch { return false; }
        return true;
    }

    private async Task<bool> SaveAllModifiedFiles()
    {
        foreach (var tab in Files.Where(t => t.IsModified).ToList())
        {
            if (!await SaveTab(tab)) return false;
        }
        if (CurrentFolderPath != null) FindPreludeFile(CurrentFolderPath);
        return true;
    }

    [RelayCommand]
    private async Task ChangeDestPath()
    {
        if (SelectedTab == null || StorageProvider == null) return;
        var file = await StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
        {
            Title = "Set Destination Output",
            DefaultExtension = "cpp",
            FileTypeChoices = [new FilePickerFileType("C++ Source") { Patterns = ["*.cpp"] }]
        });
        if (file != null) SelectedTab.OutFilePath = file.Path.LocalPath;
    }

    [RelayCommand]
    private async Task SelectTranspiler()
    {
        if (StorageProvider == null) return;
        var result = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Select Transpiler Executable",
            AllowMultiple = false,
            FileTypeFilter = [new FilePickerFileType("Executables")
            {
                Patterns = RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? ["*.exe"] : ["*"]
            }]
        });
        if (result.Count >= 1) TranspilerExePath = result[0].Path.LocalPath;
    }

    [RelayCommand]
    private async Task Translate()
    {
        if (!string.IsNullOrEmpty(CurrentFolderPath)) { if (!await SaveAllModifiedFiles()) return; }
        else if (SelectedTab != null) {
            if (SelectedTab.IsModified || string.IsNullOrEmpty(SelectedTab.FilePath)) { await SaveSource(); if (SelectedTab.IsModified) return; }
        } else return;

        Debug.WriteLine($"Translating {SelectedTab?.FilePath}...");
        await RunExternalTool("T");
    }

    [RelayCommand]
    private async Task Compile()
    {
        if (!string.IsNullOrEmpty(CurrentFolderPath)) { if (!await SaveAllModifiedFiles()) return; }
        else if (SelectedTab != null) {
            if (SelectedTab.IsModified || string.IsNullOrEmpty(SelectedTab.FilePath)) { await SaveSource(); if (SelectedTab.IsModified) return; }
        } else return;
        Debug.WriteLine("Compiling...");
        await RunExternalTool("C");
    }

    [RelayCommand]
    private async Task Run()
    {
        if (!string.IsNullOrEmpty(CurrentFolderPath)) { if (!await SaveAllModifiedFiles()) return; }
        else if (SelectedTab != null) {
            if (SelectedTab.IsModified || string.IsNullOrEmpty(SelectedTab.FilePath)) { await SaveSource(); if (SelectedTab.IsModified) return; }
        } else return;
        Debug.WriteLine("Running...");
        await RunExternalTool("R");
    }
    
    private async Task RunLSP()
    {
        if (_isLspRunning) return; 
        if (SelectedTab == null) return;
        if (!File.Exists(TranspilerExePath)) return;

        _isLspRunning = true;
        string tempPath = "";
        bool isTemp = false;

        try
        {
            // Always write to temp file so we get realtime updates even if not saved
            tempPath = Path.GetTempFileName(); 
            isTemp = true;
            await File.WriteAllTextAsync(tempPath, SelectedTab.Content);

            const string mode = "LSP";
            string outFilePath = Path.ChangeExtension(tempPath, ".cpp");

            var arguments = $"\"{tempPath}\" {mode} \"{outFilePath}\"";

            var psi = new ProcessStartInfo
            {
                FileName = TranspilerExePath,
                Arguments = arguments,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true
            };

            using var process = new Process();
            process.StartInfo = psi;

            var jsonOutput = new System.Text.StringBuilder();

            process.OutputDataReceived += (_, e) =>
            {
                if (!string.IsNullOrEmpty(e.Data)) jsonOutput.Append(e.Data);
            };

            process.ErrorDataReceived += (_, e) => { };

            process.Start();
            process.BeginOutputReadLine();
            process.BeginErrorReadLine();

            await process.WaitForExitAsync();

            var resultJson = jsonOutput.ToString();
            
            if (!string.IsNullOrWhiteSpace(resultJson) && resultJson.Trim().StartsWith("["))
            {
                SelectedTab.DiagnosticsJson = resultJson;
            }
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"LSP Error: {ex.Message}");
        }
        finally
        {
            if (isTemp && File.Exists(tempPath))
            {
                try { File.Delete(tempPath); } catch { }
            }
            _isLspRunning = false;
        }
    }
    
    private async Task RunExternalTool(string mode)
    {
        if (SelectedTab == null) { await OpenSourceFile(); if (SelectedTab == null) return; }
        if (!File.Exists(TranspilerExePath)) { await SelectTranspiler(); if (!File.Exists(TranspilerExePath)) return; }

        string? sourceFile = null;
        if (!string.IsNullOrEmpty(CurrentFolderPath)) sourceFile = FindPreludeFile(CurrentFolderPath);
        if (string.IsNullOrEmpty(sourceFile)) sourceFile = SelectedTab?.FilePath;
        if (string.IsNullOrEmpty(sourceFile)) { Terminal.Append("> Error: No file to process.\n"); return; }
        
        string outFilePath;
        if (!string.IsNullOrEmpty(CurrentFolderPath)) outFilePath = Path.ChangeExtension(sourceFile, ".cpp");
        else if (SelectedTab != null && !string.IsNullOrEmpty(SelectedTab.OutFilePath)) outFilePath = SelectedTab.OutFilePath;
        else outFilePath = Path.ChangeExtension(sourceFile, ".cpp");

        var arguments = $"\"{sourceFile}\" {mode} \"{outFilePath}\"";
        Terminal.Append($"\n> {Path.GetFileName(TranspilerExePath)} {arguments}\n");

        var psi = new ProcessStartInfo
        {
            FileName = TranspilerExePath,
            Arguments = arguments,
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
            CreateNoWindow = true
        };

        using var process = new Process();
        process.StartInfo = psi;
        process.OutputDataReceived += (_, e) => { if (e.Data != null) Terminal.Append(e.Data + "\n"); };
        process.ErrorDataReceived += (_, e) => { if (e.Data != null) Terminal.Append("\x1b[31m" + e.Data + "\x1b[0m\n"); };
        process.Start();
        process.BeginOutputReadLine();
        process.BeginErrorReadLine();
        await process.WaitForExitAsync();
        Terminal.Append($"> Process exited with code {process.ExitCode}\n");
    }
    
    [RelayCommand] private void ClearTerminalOutput() => Terminal.Clear();
    [ObservableProperty] private double _editorFontSize = 14;
    [RelayCommand] private void ZoomIn() { if (EditorFontSize < 100) EditorFontSize += 2; }
    [RelayCommand] private void ZoomOut() { if (EditorFontSize > 2) EditorFontSize -= 2; }

    [RelayCommand]
    private async Task OpenFileFromList(string fileName)
    {
        if (string.IsNullOrEmpty(CurrentFolderPath)) return;
        var fullPath = Directory.GetFiles(CurrentFolderPath, fileName, SearchOption.TopDirectoryOnly).FirstOrDefault();
        if (string.IsNullOrEmpty(fullPath) || !File.Exists(fullPath)) return;
        
        var existingTab = Files.FirstOrDefault(f => f.FilePath == fullPath);
        if (existingTab != null) { SelectedTab = existingTab; return; }

        try { var content = await File.ReadAllTextAsync(fullPath); AddTab(fileName, content, fullPath); }
        catch (Exception ex) { Debug.WriteLine($"Error opening file: {ex.Message}"); }
    }

    private void AddTab(string header, string content)
    {
        var newTab = new FileTabViewModel(header, content, CloseTab);
        Files.Add(newTab);
        SelectedTab = newTab;
    }

    private void AddTab(string header, string content, string filePath)
    {
        var newTab = new FileTabViewModel(header, content, CloseTab) { FilePath = filePath, IsModified = false };
        Files.Add(newTab);
        SelectedTab = newTab;
    }

    private void CloseTab(FileTabViewModel tab)
    {
        if (!Files.Contains(tab)) return;
        Files.Remove(tab);
        if (SelectedTab == tab || SelectedTab == null) SelectedTab = Files.LastOrDefault();
    }
}