Feature: Logger canonical interleaved output
  As a developer
  I want Logger to write canonical 2N-line artifacts
  So that blank payloads are represented by blank lines and append concatenates records

  Scenario: Write and append mixed-payload records
    Given a temporary artifact file
    When I write 3 records with payload presence pattern "101"
    Then the file has 6 lines with blanks at positions "2"
    When I append the same 3 records to the file
    Then the file now has 12 lines
