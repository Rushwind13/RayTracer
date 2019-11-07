# language: en
Feature: Tuples
  So that positions will work properly
  As a ray tracer
  I want to test many things about vector math

  Scenario: add vector to position
    Given I have a position 1,0,0 in the data
    And I have a vector 0,1,0 in the data
    When I press add
    Then the result should be 1,1,0 on the screen
