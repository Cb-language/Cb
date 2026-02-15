using System.Text.Json.Serialization;

namespace composer.LSP
{
    public class Position
    {
        [JsonPropertyName("line")]
        public int Line { get; set; }

        [JsonPropertyName("character")]
        public int Character { get; set; }
    }
}
