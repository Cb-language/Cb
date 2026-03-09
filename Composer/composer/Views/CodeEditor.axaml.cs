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

    // Auto-Complete Pairs
    private readonly Dictionary<string, string> _bracketPairs = new()
    {
        { "[", "]" },
        { "{", "}" },
        { "\"", "\"" },
        { "'", "'" },
        { "𝄞", "𝄀" },
        { "𝄋", "𝄌"},
        { "𝄕", "𝄇"}
    };

    public CodeEditor()
    {
        InitializeComponent();
        LoadSyntax();

        Editor.Options.ConvertTabsToSpaces = true;
        Editor.Options.IndentationSize = 4;

        _textMarkerService = new TextMarkerService(Editor);
        Editor.TextArea.TextView.BackgroundRenderers.Add(_textMarkerService);
        Editor.TextArea.TextView.LineTransformers.Add(_textMarkerService);

        // Events
        Editor.TextArea.KeyDown += OnEditorKeyDown;
        Editor.TextArea.PointerMoved += OnEditorPointerMoved;
        Editor.TextArea.TextEntered += OnTextEntered;
        Editor.TextArea.TextEntering += OnTextEntering;

        // ReSharper disable once UnusedParameter.Local
        Editor.TextChanged += (sender, args) =>
        {
            if (_isBinding) return;
            _isBinding = true;
            CodeContent = Editor.Text;
            _isBinding = false;
        };
    }

    private void OnTextEntering(object? sender, TextInputEventArgs e)
    {
        if (string.IsNullOrEmpty(e.Text)) return;

        string? replacement = null;

        switch (e.Text)
        {
            case "{": replacement = "𝄋"; break;
            case "}": replacement = "𝄌"; break;
            case "(": replacement = "𝄕"; break;
            case ")": replacement = "𝄇"; break;
            case "&": replacement = "𝄞"; break;
            case ";": replacement = "𝄀"; break;
            case "~": replacement = "𝄽"; break;
            case "^": replacement = "𝆫"; break;
            case "\\": replacement = "𝄍"; break;
            case "#": replacement = "♯"; break;
        }

        if (replacement != null)
        {
            e.Handled = true;
            Editor.TextArea.Selection.ReplaceSelectionWithText(replacement);
            HandleAutoCompletion(replacement);
        }
    }

    private void OnTextEntered(object? sender, TextInputEventArgs e)
    {
        if (e.Text != null) HandleAutoCompletion(e.Text);
    }

    private void HandleAutoCompletion(string text)
    {
        if (string.IsNullOrEmpty(text)) return;

        if (_bracketPairs.TryGetValue(text, out var closingChar))
        {
            Editor.Document.Insert(Editor.CaretOffset, closingChar);
            Editor.CaretOffset--; 
        }
    }

    private void OnEditorPointerMoved(object? sender, PointerEventArgs e)
    {
        var textView = Editor.TextArea.TextView;
        var pos = e.GetPosition(textView);
        var position = textView.GetPosition(pos);
        
        if (position.HasValue)
        {
            var offset = Editor.Document.GetOffset(position.Value.Line, position.Value.Column);
            var errorMsg = _textMarkerService.GetErrorAtOffset(offset);

            if (!string.IsNullOrEmpty(errorMsg))
            {
                ToolTip.SetTip(Editor, errorMsg);
                ToolTip.SetIsOpen(Editor, true);
            }
            else
            {
                ToolTip.SetIsOpen(Editor, false);
            }
        }
        else
        {
            ToolTip.SetIsOpen(Editor, false);
        }
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
                            // Filter out "no prelude" errors as they are often irrelevant in multi-file contexts
                            allDiagnostics.AddRange(diagnosticParams.Diagnostics.Where(d => d.Message != "no prelude"));
                        }
                    }
                }
            }
        
            _textMarkerService.UpdateMarkers(allDiagnostics);
        }
        catch 
        { 
            _textMarkerService.UpdateMarkers(new List<Diagnostic>());
        }
    }

    private void OnEditorKeyDown(object? sender, KeyEventArgs e)
    {
        // 1. Handle Comment Toggle (Ctrl + /)
        if (e.KeyModifiers == KeyModifiers.Control && e.Key == Key.OemQuestion)
        {
            ToggleComment();
            e.Handled = true;
            return;
        }

        // 2. Handle Alt Keymaps
        if (e.KeyModifiers.HasFlag(KeyModifiers.Alt))
        {
            string? replacement = null;
            if (e.KeyModifiers.HasFlag(KeyModifiers.Shift))
            {
                switch (e.Key)
                {
                    case Key.B: replacement = "𝄫"; break;       // ALT + SHIFT + B
                }
            }
            else
            {
                switch (e.Key)
                {
                    case Key.OemTilde: replacement = "𝄡"; break; // ALT + ~ (or ` key)
                    case Key.D0: replacement = "♮"; break; // ALT + 0
                    case Key.C: replacement = "©"; break; // ALT + C
                    case Key.OemQuestion: // ALT + / (or ? key)
                    case Key.Divide: replacement = "𝄂"; break; // ALT + Numpad /
                    case Key.OemPlus: replacement = "𝅘𝅥="; break; // ALT + Numpad +
                    case Key.D3: replacement = "𝄪"; break;  // ALT+ #
                    case Key.B: replacement = "♭"; break; // ALT + B

                }
            }

            if (replacement != null)
            {
                e.Handled = true;
                Editor.TextArea.Selection.ReplaceSelectionWithText(replacement);
                HandleAutoCompletion(replacement);
            }
        }
    }

    private void ToggleComment()
    {
        var document = Editor.Document;
        const string commentPrefix = "?";

        // 1. Single Line Logic (No Selection)
        if (Editor.SelectionLength == 0)
        {
            var offset = Editor.CaretOffset;
            var line = document.GetLineByOffset(offset);
            var lineText = document.GetText(line);
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
            return;
        }

        // 2. Multi-Line Logic
        var selectionStart = Editor.SelectionStart;
        var selectionEnd = Editor.SelectionStart + Editor.SelectionLength;

        var startLine = document.GetLineByOffset(selectionStart);
        var endLine = document.GetLineByOffset(selectionEnd);

        // If selection ends at the very start of a line (e.g. user selected full lines), 
        // exclude that last empty line from operation
        if (endLine.Offset == selectionEnd && endLine.LineNumber > startLine.LineNumber)
        {
            endLine = document.GetLineByNumber(endLine.LineNumber - 1);
        }

        // Check if ALL lines are already commented
        bool allCommented = true;
        for (int i = startLine.LineNumber; i <= endLine.LineNumber; i++)
        {
            var line = document.GetLineByNumber(i);
            var text = document.GetText(line).TrimStart();
            if (text.Length > 0 && !text.StartsWith(commentPrefix))
            {
                allCommented = false;
                break;
            }
        }

        document.BeginUpdate();
        try
        {
            for (int i = startLine.LineNumber; i <= endLine.LineNumber; i++)
            {
                var line = document.GetLineByNumber(i);
                var text = document.GetText(line);
                var trimmed = text.TrimStart();
                var leadingSpaces = text.Length - trimmed.Length;

                if (allCommented)
                {
                    // Uncomment: Remove '?' if it exists
                    if (trimmed.StartsWith(commentPrefix))
                    {
                        document.Remove(line.Offset + leadingSpaces, commentPrefix.Length);
                    }
                }
                else
                {
                    // Comment: Add '?' (only if line is not empty, or forces it)
                    // Usually we comment empty lines too for visual block consistency
                    document.Insert(line.Offset + leadingSpaces, commentPrefix);
                }
            }
        }
        finally
        {
            document.EndUpdate();
        }
    }

    private void LoadSyntax()
    {
        const string resourceUri = "avares://composer/Assets/MusicSyntax.xshd";
        try 
        {
            var uri = new Uri(resourceUri);
            using var stream = AssetLoader.Open(uri);
            using var reader = XmlReader.Create(stream);
            var definition = HighlightingLoader.Load(reader, HighlightingManager.Instance);
            
            // Register it so it can be found by name if needed
            HighlightingManager.Instance.RegisterHighlighting("MusicLang", new[] { ".music" }, definition);
            
            Editor.SyntaxHighlighting = definition;
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Error loading syntax: {ex.Message}");
        }
    }
}