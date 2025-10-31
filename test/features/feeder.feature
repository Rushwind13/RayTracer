Feature: Feeder emits correct interleaved records
  As a developer
  I want Feeder to treat blank Intersection lines as empty payloads
  So that downstream components see one Pixel+Intersection pair per record

  Background:
    Given I prepare a sample interleaved file with a blank Intersection in the middle at "runs/tests/feeder_sample.txt"

  @skip
  Scenario: Blank intersection line yields empty Intersection in output
    When I capture Feeder output from "runs/tests/feeder_sample.txt" into "runs/tests/FeederCapture.txt"
    Then the output file "runs/tests/oFeederCapture.txt" should exist with at least 3 records
    And in capture "runs/tests/oFeederCapture.txt" record 1 intersection oid equals 5 and gothit equals 1
    And in capture "runs/tests/oFeederCapture.txt" record 2 intersection oid equals -1 and gothit equals 0
    And in capture "runs/tests/oFeederCapture.txt" record 3 intersection oid equals 7 and gothit equals 1
