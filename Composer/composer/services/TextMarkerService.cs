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

        // --- 1. Draw Red Squiggly Line (Under whole line) ---
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

        // --- 2. Colorize Text Red (Whole line) ---
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

        // --- 3. Update Logic ---
        public void UpdateMarkers(IEnumerable<Diagnostic> diagnostics)
        {
            if (_editor.Document == null) return;
            
            _markers.Clear();

            foreach (var diagnostic in diagnostics)
            {
                if (diagnostic.Range.Start.Line >= _editor.Document.LineCount) continue;

                try 
                {
                    // 1-based indexing for AvaloniaEdit
                    int lineNum = diagnostic.Range.Start.Line + 1;
                    if (lineNum < 1 || lineNum > _editor.Document.LineCount) continue;

                    var line = _editor.Document.GetLineByNumber(lineNum);
                    
                    // Create marker for WHOLE LINE content
                    if (line.Length > 0)
                    {
                        _markers.Add(new TextMarker(line.Offset, line.Length, diagnostic.Message));
                    }
                }
                catch (Exception) { /* Ignore */ }
            }
            
            // Force Redraw to ensure removed errors disappear
            _editor.TextArea.TextView.Redraw();
            _editor.TextArea.TextView.InvalidateVisual();
        }

        public string? GetErrorAtOffset(int offset)
        {
            if (_markers == null) return null;
            // FindSegmentsContaining returns segments where Start <= offset < End
            var marker = _markers.FindSegmentsContaining(offset).FirstOrDefault();
            return marker?.Message;
        }
    }

    public class TextMarker : TextSegment
    {
        public string Message { get; }

        public TextMarker(int startOffset, int length, string message)
        {
            StartOffset = startOffset;
            Length = length;
            Message = message;
        }
    }
}