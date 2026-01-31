using System.Collections.Generic;
using Avalonia.Media;
using System.Collections.ObjectModel;
using System.Text;

namespace composer.ANSIHelper;

public sealed class AnsiTerminal
{
    public ObservableCollection<TerminalSpan> Spans { get; } = new();

    private IBrush _currentColor = Brushes.LightGray;

    private static readonly Dictionary<int, IBrush> AnsiColors = new()
    {
        { 30, Brushes.Black },
        { 31, Brushes.Red },
        { 32, Brushes.Green },
        { 33, Brushes.Yellow },
        { 34, Brushes.Blue },
        { 35, Brushes.Magenta },
        { 36, Brushes.Cyan },
        { 37, Brushes.White },
    };

    public void Append(string input)
    {
        var buffer = new StringBuilder();

        for (int i = 0; i < input.Length; i++)
        {
            if (input[i] == '\x1b' && i + 1 < input.Length && input[i + 1] == '[')
            {
                Flush(buffer);

                int mIndex = input.IndexOf('m', i);
                if (mIndex == -1)
                    return;

                ParseAnsi(input.Substring(i + 2, mIndex - i - 2));
                i = mIndex;
            }
            else
            {
                buffer.Append(input[i]);
            }
        }

        Flush(buffer);
    }

    public void Clear()
    {
        Spans.Clear();
        _currentColor = Brushes.LightGray;
    }

    private void Flush(StringBuilder buffer)
    {
        if (buffer.Length == 0)
            return;

        Spans.Add(new TerminalSpan(buffer.ToString(), _currentColor));
        buffer.Clear();
    }

    private void ParseAnsi(string sequence)
    {
        foreach (var part in sequence.Split(';'))
        {
            if (!int.TryParse(part, out int code))
                continue;

            if (code == 0)
            {
                _currentColor = Brushes.LightGray;
            }
            else if (AnsiColors.TryGetValue(code, out var brush))
            {
                _currentColor = brush;
            }
        }
    }
}