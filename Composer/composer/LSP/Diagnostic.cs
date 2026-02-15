using System.Text.Json.Serialization;

namespace composer.LSP
{
    public class Diagnostic
    {
        [JsonPropertyName("range")]
        public Range Range { get; set; }

        [JsonPropertyName("severity")]
        public int Severity { get; set; }

        [JsonPropertyName("source")]
        public string Source { get; set; }

        [JsonPropertyName("message")]
        public string Message { get; set; }
    }
}
