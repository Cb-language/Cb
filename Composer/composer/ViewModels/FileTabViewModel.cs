using System;
using System.IO;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

namespace composer.ViewModels;

public partial class FileTabViewModel : ViewModelBase
{
    [ObservableProperty] private string _header = "Untitled";
    [ObservableProperty] private string? _filePath;
    [ObservableProperty] private string? _outFilePath;
    [ObservableProperty] private bool _isModified;
    
    // Ensure two-way binding works nicely with the editor
    [ObservableProperty] private string _content = "";
    
    [ObservableProperty] private string _diagnosticsJson = "";

    public IRelayCommand CloseTabCommand { get; }

    public FileTabViewModel(string header, string content, Action<FileTabViewModel> closeCallback)
    {
        Header = header;
        Content = content;
        CloseTabCommand = new RelayCommand(() => closeCallback(this));
    }

    partial void OnFilePathChanged(string? value)
    {
        Header = string.IsNullOrEmpty(value) ? "Untitled" : Path.GetFileName(value);
        if (!string.IsNullOrEmpty(value)) OutFilePath = Path.ChangeExtension(value, ".cpp");
    }

    // ReSharper disable once UnusedParameterInPartialMethod
    partial void OnContentChanged(string value)
    {
        IsModified = true; 
    }
}