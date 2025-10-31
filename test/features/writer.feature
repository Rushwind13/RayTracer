Feature: Writer renders PNG from interleaved records
  As a developer
  I want Writer to ignore the second payload and stride over Pixel+Intersection pairs
  So that no editing of o*.txt files is needed to render a PNG

  Background:
    Given I prepare a sample interleaved file with a blank Intersection in the middle at "runs/tests/feeder_sample.txt"

  Scenario: Render PNG from interleaved artifact with blank intersections
    When I render a PNG via Echo from artifact "runs/tests/feeder_sample.txt" to "runs/tests/sample.png"
    Then a PNG "runs/tests/sample.png" should exist
    And Writer reported at least 1 distinct colors in "runs/tests/writer_echo.log"
