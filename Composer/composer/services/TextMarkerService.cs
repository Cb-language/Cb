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
    // Removed ITextViewConnect to prevent StackOverflow
    public class TextMarkerService : IBackgroundRenderer
    {
        private readonly TextEditor _editor;
        private readonly TextSegmentCollection<TextMarker> _markers;

        public TextMarkerService(TextEditor editor)
        {
            _editor = editor;
            _markers = new TextSegmentCollection<TextMarker>(_editor.Document);
        }

        public KnownLayer Layer => KnownLayer.Selection;

        public void Draw(TextView textView, DrawingContext drawingContext)
        {
            if (_markers == null || !_markers.Any() || textView.VisualLines.Count == 0) return;

            var visualLines = textView.VisualLines;
            if (visualLines.Count == 0) return;

            var viewStart = visualLines.First().StartOffset;
            
            // Safe way to get the end offset
            var lastLine = visualLines.Last();
            var viewEnd = lastLine.LastDocumentLine.EndOffset;

            foreach (var marker in _markers.FindOverlappingSegments(viewStart, viewEnd))
            {
                foreach (var r in BackgroundGeometryBuilder.GetRectsForSegment(textView, marker))
                {
                    var startPoint = r.BottomLeft;
                    var endPoint = r.BottomRight;

                    var pen = new Pen(Brushes.Red, 1);
                    var geometry = new StreamGeometry();
                    
                    using (var ctx = geometry.Open())
                    {
                        double x = startPoint.X;
                        double y = startPoint.Y - 2; 
                        double waveSize = 3.0; 
                        
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