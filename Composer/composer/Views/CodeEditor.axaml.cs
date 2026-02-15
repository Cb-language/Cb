using System;
using System.Linq;
using System.Text.Json;
using System.Xml;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input; // [Added] Needed for KeyEventArgs and Key modifiers
using Avalonia.Platform;
using AvaloniaEdit.Highlighting;
using AvaloniaEdit.Highlighting.Xshd;
using composer.LSP;
using composer.services;

namespace composer.Views;

public partial class CodeEditor : UserControl
{
    public static readonly StyledProperty<string> CodeContentProperty =
        AvaloniaProperty.Register<CodeEditor, string>(nameof(CodeContent), defaultBindingMode: Avalonia.Data.BindingMode.TwoWay);

    public string CodeContent
    {
        get => GetValue(CodeContentProperty);
        set => SetValue(CodeContentProperty, value);
    }

    private bool _isBinding;
    private readonly TextMarkerService _textMarkerService;

    public CodeEditor()
    {
        InitializeComponent();
        LoadSyntax();

        Editor.Options.ConvertTabsToSpaces = true;
        Editor.Options.IndentationSize = 4;

        _textMarkerService = new TextMarkerService(Editor);
        Editor.TextArea.TextView.BackgroundRenderers.Add(_textMarkerService);

        // [Added] Hook up the KeyDown event to handle shortcuts
        Editor.TextArea.KeyDown += OnEditorKeyDown;

        Editor.TextChanged += (sender, args) =>
        {
            if (_isBinding) return;
            _isBinding = true;
            CodeContent = Editor.Text;
            _isBinding = false;
        };
    }

    // [Added] Key Handler
    private void OnEditorKeyDown(object? sender, KeyEventArgs e)
    {
        // Check for Ctrl + / (OemQuestion is usually the / key)
        if (e.KeyModifiers == KeyModifiers.Control && e.Key == Key.OemQuestion)
        {
            ToggleComment();
            e.Handled = true; // Mark as handled so it doesn't type '/'
        }
    }

    // [Added] Toggle Comment Logic
    private void ToggleComment()
    {
        var document = Editor.Document;
        var offset = Editor.CaretOffset;
        // Get the line where the cursor currently is
        var line = document.GetLineByOffset(offset);
        var lineText = document.GetText(line);
        
        // Based on your MusicSyntax.xshd, '?' is the single-line comment
        const string commentPrefix = "?"; 

        // Find where the code actually starts (skip indentation)
        var trimmed = lineText.TrimStart();
        var leadingSpaceCount = lineText.Length - trimmed.Length;

        if (trimmed.StartsWith(commentPrefix))
        {
            // UNCOMMENT: Remove the '?'
            // We calculate the exact position of the '?'
            var commentIndex = line.Offset + leadingSpaceCount;
            document.Remove(commentIndex, commentPrefix.Length);
        }
        else
        {
            // COMMENT: Insert '?' at the indentation level
            var insertOffset = line.Offset + leadingSpaceCount;
            document.Insert(insertOffset, commentPrefix);
        }
    }

    public void HighlightErrors(string json)
    {
        if (string.IsNullOrEmpty(json)) return;

        try 
        {
            var root = JsonSerializer.Deserialize<JsonElement>(json);
            if (root.ValueKind != JsonValueKind.Array) return;

            var allDiagnostics = new System.Collections.Generic.List<Diagnostic>();

            foreach (var message in root.EnumerateArray())
            {
                if (message.TryGetProperty("method", out var method) && 
                    method.GetString() == "textDocument/publishDiagnostics")
                {
                    if (message.TryGetProperty("params", out var paramsElement))
                    {
                        var diagnosticParams = JsonSerializer.Deserialize<DiagnosticParams>(paramsElement.GetRawText());
                        if (diagnosticParams?.Diagnostics != null)
                        {
                            allDiagnostics.AddRange(diagnosticParams.Diagnostics);
                        }
                    }
                }
            }
        
            _textMarkerService.UpdateMarkers(allDiagnostics);
        }
        catch (Exception ex) 
        { 
            System.Diagnostics.Debug.WriteLine($"LSP Parse Error: {ex.Message}"); 
        }
    }

    protected override void OnPropertyChanged(AvaloniaPropertyChangedEventArgs change)
    {
        base.OnPropertyChanged(change);

        if (change.Property != CodeContentProperty) return;
        if (_isBinding) return;
            
        var newVal = change.GetNewValue<string>();

        if (Editor.Text == newVal) return;
        _isBinding = true;
        Editor.Text = newVal;
        _isBinding = false;
    }

    private void LoadSyntax()
    {
        var resourceUri = "avares://composer/Assets/MusicSyntax.xshd";
        try 
        {
            var uri = new Uri(resourceUri);
            if (!AssetLoader.Exists(uri)) return;
            using var stream = AssetLoader.Open(uri);
            using var reader = new XmlTextReader(stream);
            Editor.SyntaxHighlighting = HighlightingLoader.Load(reader, HighlightingManager.Instance);
        }
        catch (Exception ex) { System.Diagnostics.Debug.WriteLine($"Syntax Load Error: {ex.Message}"); }
    }
}