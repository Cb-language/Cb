namespace composer.ANSIHelper;

using Avalonia.Media;

public sealed class TerminalSpan(string text, IBrush foreground)
{
    public string Text { get; } = text;
    public IBrush Foreground { get; } = foreground;
}
