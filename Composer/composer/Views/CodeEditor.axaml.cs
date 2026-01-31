using System;
using System.Xml;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Platform;
using AvaloniaEdit.Highlighting;
using AvaloniaEdit.Highlighting.Xshd;

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

    private bool _isBinding = false;

    public CodeEditor()
    {
        InitializeComponent();
        LoadSyntax();

        Editor.Options.ConvertTabsToSpaces = true;
        Editor.Options.IndentationSize = 4;

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

        if (change.Property != CodeContentProperty) return;
        if (_isBinding) return;
            
        var newVal = change.GetNewValue<string>() ?? "";

        if (Editor.Text == newVal) return;
        _isBinding = true;
        Editor.Text = newVal;
        _isBinding = false;
    }

    private void LoadSyntax()
    {
        // Ensure this path matches your actual asset location
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