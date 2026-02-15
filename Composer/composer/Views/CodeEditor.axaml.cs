using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Xml;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
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

    public static readonly StyledProperty<string> DiagnosticsJsonProperty =
        AvaloniaProperty.Register<CodeEditor, string>(nameof(DiagnosticsJson));

    public string DiagnosticsJson
    {
        get => GetValue(DiagnosticsJsonProperty);
        set => SetValue(DiagnosticsJsonProperty, value);
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
        Editor.TextArea.TextView.LineTransformers.Add(_textMarkerService);

        Editor.TextArea.KeyDown += OnEditorKeyDown;

        Editor.TextChanged += (sender, args) =>
        {
            if (_isBinding) return;
            _isBinding = true;
            CodeContent = Editor.Text;
            _isBinding = false;
        };
    }

    protected override void OnPropertyChanged(AvaloniaPropertyChangedEventArgs change)
    {
        base.OnPropertyChanged(change);

        if (change.Property == CodeContentProperty)
        {
            if (_isBinding) return;
            var newVal = change.GetNewValue<string>();
            if (Editor.Text == newVal) return;
            _isBinding = true;
            Editor.Text = newVal;
            _isBinding = false;
        }
        else if (change.Property == DiagnosticsJsonProperty)
        {
            var json = change.GetNewValue<string>();
            HighlightErrors(json);
        }
    }

    public void HighlightErrors(string json)
    {
        // Even if empty, we MUST call UpdateMarkers to clear old red lines
        if (string.IsNullOrEmpty(json) || json == "[]") 
        {
            _textMarkerService.UpdateMarkers(new List<Diagnostic>());
            return;
        }

        try 
        {
            var root = JsonSerializer.Deserialize<JsonElement>(json);
            if (root.ValueKind != JsonValueKind.Array) 
            {
                 _textMarkerService.UpdateMarkers(new List<Diagnostic>());
                 return;
            }

            var allDiagnostics = new List<Diagnostic>();

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
        catch 
        { 
            // On error, clear markers to avoid stuck graphics
            _textMarkerService.UpdateMarkers(new List<Diagnostic>());
        }
    }

    private void OnEditorKeyDown(object? sender, KeyEventArgs e)
    {
        if (e.KeyModifiers == KeyModifiers.Control && e.Key == Key.OemQuestion)
        {
            ToggleComment();
            e.Handled = true;
        }
    }

    private void ToggleComment()
    {
        var document = Editor.Document;
        var offset = Editor.CaretOffset;
        var line = document.GetLineByOffset(offset);
        var lineText = document.GetText(line);
        const string commentPrefix = "?"; 

        var trimmed = lineText.TrimStart();
        var leadingSpaceCount = lineText.Length - trimmed.Length;

        if (trimmed.StartsWith(commentPrefix))
        {
            var commentIndex = line.Offset + leadingSpaceCount;
            document.Remove(commentIndex, commentPrefix.Length);
        }
        else
        {
            var insertOffset = line.Offset + leadingSpaceCount;
            document.Insert(insertOffset, commentPrefix);
        }
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
        catch { }
    }
}