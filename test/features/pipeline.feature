Feature: Pipeline smoke test
  As a developer
  I want a quick end-to-end smoke
  So that I can verify the distributed pipeline produces an image

  Scenario: End-to-end produces an image
    Given I run the all-up script
    Then a PNG "bin/test.png" should exist
