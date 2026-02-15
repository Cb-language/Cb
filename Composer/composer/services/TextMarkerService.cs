using System;
using System.Collections.Generic;
using System.Linq;
using Avalonia;
using Avalonia.Media;
using AvaloniaEdit;
using AvaloniaEdit.Document;
using AvaloniaEdit.Rendering;
using composer.LSP;

namespace composer.services
{
    public class TextMarkerService : IBackgroundRenderer, ITextViewConnect
    {
        private readonly TextEditor _editor;
        private readonly TextSegmentCollection<TextMarker> _markers;

        public TextMarkerService(TextEditor editor)
        {
            _editor = editor;
            _markers = new TextSegmentCollection<TextMarker>(_editor.Document);
        }

        public void AddToTextView(TextView textView)
        {
            if (!textView.BackgroundRenderers.Contains(this))
                textView.BackgroundRenderers.Add(this);
        }

        public void RemoveFromTextView(TextView textView)
        {
            textView.BackgroundRenderers.Remove(this);
        }

        public KnownLayer Layer => KnownLayer.Selection;

        public void Draw(TextView textView, DrawingContext drawingContext)
        {
            if (_markers == null || !_markers.Any() || textView.VisualLines.Count == 0) return;

            var visualLines = textView.VisualLines;
            if (visualLines.Count == 0) return;

            var viewStart = visualLines.First().StartOffset;
            var viewEnd = visualLines.Last().StartOffset + visualLines.Last().TextLength;

            foreach (var marker in _markers.FindOverlappingSegments(viewStart, viewEnd))
            {
                foreach (var r in BackgroundGeometryBuilder.GetRectsForSegment(textView, marker))
                {
                    var startPoint = r.BottomLeft;
                    var endPoint = r.BottomRight;

                    // Draw Squiggly Line
                    var pen = new Pen(Brushes.Red, 1);
                    var geometry = new StreamGeometry();
                    
                    using (var ctx = geometry.Open())
                    {
                        var x = startPoint.X;
                        var y = startPoint.Y - 2; // Lift slightly
                        const double waveSize = 3.0; 
                        
                        ctx.BeginFigure(new Point(x, y), false);
                        
                        while (x < endPoint.X)
                        {
                            ctx.LineTo(new Point(x + waveSize / 2, y + waveSize));
                            ctx.LineTo(new Point(x + waveSize, y));
                            x += waveSize;
                        }
                    }
                    drawingContext.DrawGeometry(null, pen, geometry);
                }
            }
        }

        public void UpdateMarkers(IEnumerable<Diagnostic> diagnostics)
        {
            if (_editor.Document == null) return;
            
            _markers.Clear();
            foreach (var diagnostic in diagnostics)
            {
                if (diagnostic.Range.Start.Line >= _editor.Document.LineCount) continue;

                try 
                {
                    // LSP uses 0-based indexing. AvaloniaEdit uses 1-based indexing for GetOffset(line, col).
                    // We add 1 to line and character to match AvaloniaEdit's expectation.
                    var startOffset = _editor.Document.GetOffset(
                        diagnostic.Range.Start.Line + 1, 
                        diagnostic.Range.Start.Character + 1
                    );

                    var endOffset = _editor.Document.GetOffset(
                        diagnostic.Range.End.Line + 1, 
                        diagnostic.Range.End.Character + 1
                    );
                    
                    if (startOffset < endOffset)
                    {
                         _markers.Add(new TextMarker(startOffset, endOffset - startOffset));
                    }
                }
                catch (ArgumentOutOfRangeException) { /* Ignore out of bounds */ }
            }
            _editor.TextArea.TextView.InvalidateLayer(KnownLayer.Selection);
        }
    }

    public class TextMarker : TextSegment
    {
        public TextMarker(int startOffset, int length)
        {
            StartOffset = startOffset;
            Length = length;
        }
    }
}