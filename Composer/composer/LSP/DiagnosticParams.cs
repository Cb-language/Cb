using System.Text.Json.Serialization;

namespace composer.LSP
{
    public class DiagnosticParams
    {
        [JsonPropertyName("uri")]
        public string Uri { get; set; }

        [JsonPropertyName("diagnostics")]
        public Diagnostic[] Diagnostics { get; set; }
    }
}
