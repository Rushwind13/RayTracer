Feature: Logger always writes interleaved pairs
  As a developer
  I want Logger to capture Pixel streams with occasional missing payloads into interleaved files
  So that artifacts remain consistent for downstream tools

  Background:
    Given I prepare a sample interleaved file with a blank Intersection in the middle at "runs/tests/feeder_sample.txt"

  Scenario: Logger captures interleaved pairs from Feeder
    When I capture Feeder output from "runs/tests/feeder_sample.txt" into "runs/tests/LoggerCapture.txt"
    Then the output file "runs/tests/oLoggerCapture.txt" should exist with at least 3 records
    And in capture "runs/tests/oLoggerCapture.txt" record 2 intersection oid equals -1 and gothit equals 0