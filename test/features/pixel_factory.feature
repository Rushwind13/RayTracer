Feature: PixelFactory artifact format
  As a developer
  I want a canonical interleaved artifact from PixelFactory
  So that downstream tools can rely on 2N-line records and traversal order

  Scenario: Interleaved file with blank payloads and row-major traversal
    Given I load the sample artifact "test/fixtures/sample_3_records.txt"
    Then it has interleaved 2N lines with blank payloads
  And the first row is row-major for 3 pixels
