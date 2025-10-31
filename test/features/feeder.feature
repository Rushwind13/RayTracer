Feature: Feeder default payload and limits
  As a developer
  I want Feeder semantics to match the artifact contract
  So that blank or missing payloads become defaults and limits apply to records

  Scenario: Defaults synthesized for blank or missing payloads, limited by records
    Given I load the sample artifact "test/fixtures/sample_3_records.txt"
    When I synthesize default Intersections for blank payloads
    Then all blank payload records have default Intersection
  When I limit to 2 records
  Then the limited records count is 2
