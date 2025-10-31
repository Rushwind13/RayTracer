Feature: PixelFactory emits interleaved CSV
  As a developer
  I want PixelFactory outputs to be interleaved (Pixel + blank Intersection)
  So that downstream tools can read records by pairs

  Scenario: data/pixels.txt is interleaved
    When the input file "data/pixels.txt" should exist with at least 100 records
    Then in interleaved file "data/pixels.txt" first 50 records have blank intersection lines
