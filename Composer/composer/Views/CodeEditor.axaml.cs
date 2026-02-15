using System;
using System.Linq;
using System.Text.Json;
using System.Xml;
using Avalonia;
using Avalonia.Controls;
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


        Editor.TextChanged += (sender, args) =>
        {
            if (_isBinding) return;
            _isBinding = true;
            CodeContent = Editor.Text;
            _isBinding = false;
        };
    }
    public void HighlightErrors(string json)
    {
        if (string.IsNullOrEmpty(json)) return;

        try 
        {
            // 1. Parse the root array
            var root = JsonSerializer.Deserialize<JsonElement>(json);
            if (root.ValueKind != JsonValueKind.Array) return;

            var allDiagnostics = new System.Collections.Generic.List<Diagnostic>();

            // 2. Iterate through LSP messages
            foreach (var message in root.EnumerateArray())
            {
                // We only care about publishDiagnostics methods
                if (message.TryGetProperty("method", out var method) && 
                    method.GetString() == "textDocument/publishDiagnostics")
                {
                    if (message.TryGetProperty("params", out var paramsElement))
                    {
                        // 3. Deserialize the inner params
                        var diagnosticParams = JsonSerializer.Deserialize<DiagnosticParams>(paramsElement.GetRawText());
                        if (diagnosticParams?.Diagnostics != null)
                        {
                            allDiagnostics.AddRange(diagnosticParams.Diagnostics);
                        }
                    }
                }
            }
        
            // 4. Update the service
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