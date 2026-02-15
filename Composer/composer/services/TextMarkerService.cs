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
    public class TextMarkerService : DocumentColorizingTransformer, IBackgroundRenderer
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

        protected override void ColorizeLine(DocumentLine line)
        {
            if (_markers == null || !_markers.Any()) return;

            var markers = _markers.FindOverlappingSegments(line.Offset, line.Length);
            
            foreach (var marker in markers)
            {
                ChangeLinePart(
                    Math.Max(marker.StartOffset, line.Offset),
                    Math.Min(marker.EndOffset, line.EndOffset),
                    element => 
                    {
                        element.TextRunProperties.SetForegroundBrush(Brushes.Red);
                    }
                );
            }
        }

        public void UpdateMarkers(IEnumerable<Diagnostic> diagnostics)
        {
            if (_editor.Document == null) return;
            
            _markers.Clear();
            foreach (var diagnostic in diagnostics)
            {
                // LSP lines are 0-based, Avalonia is 1-based. So Line + 1 is correct.
                if (diagnostic.Range.Start.Line >= _editor.Document.LineCount) continue;

                try 
                {
                    var lineObj = _editor.Document.GetLineByNumber(diagnostic.Range.Start.Line + 1);
                    
                    // FIX: Treat Character as 1-based column (removing the previous +1).
                    // Also clamp to line length to prevent crashes.
                    int startCol = Math.Max(1, Math.Min(lineObj.Length + 1, diagnostic.Range.Start.Character));
                    int endCol = Math.Max(1, Math.Min(lineObj.Length + 1, diagnostic.Range.End.Character));
                    
                    // If length is 0 (e.g. insert point), give it at least 1 char width so we can see it
                    if (startCol == endCol && startCol <= lineObj.Length) 
                        endCol++;

                    var startOffset = _editor.Document.GetOffset(diagnostic.Range.Start.Line + 1, startCol);
                    var endOffset = _editor.Document.GetOffset(diagnostic.Range.End.Line + 1, endCol);
                    
                    if (startOffset < endOffset)
                    {
                         _markers.Add(new TextMarker(startOffset, endOffset - startOffset));
                    }
                }
                catch (Exception) { /* Safely ignore invalid ranges */ }
            }
            
            _editor.TextArea.TextView.Redraw();
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