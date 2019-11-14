# language: en
Feature: Rays
    In order to better understand rays
    As a ray tracer
    I want to do lots of math with Rays

    Scenario: create a Ray
        Given I have a position 0,0,0 in the data
        And I have a vector 0,0,-1 in the data
        When I press Ray
        Then the origin should be 0,0,0 a position
        Then the direction should be 0,0,-1 a vector
        Then the length should be 1.0 a float

    Scenario: Computing a point from a distance
        Given I have a position 2,3,4 in the data
        And I have a vector 1,0,0 in the data
        When I press Ray
        Then position r,0 = 2,3,4
        Then position r,1 = 3,3,4
        Then position r,-1 = 1,3,4
        Then position r,2.5 = 4.5,3,4
## Ray transforms
  Scenario: Translating a ray
    Given I have a Ray with origin 1,2,3 and direction 0,1,0
    Given I have a position 3,4,5 in the data
    And I create a translation matrix A
    When I transform the ray with A
    Then the origin should be 4,6,8 a position
    And the direction should be 0,1,0 a vector

  Scenario: Scaling a ray
    Given I have a Ray with origin 1,2,3 and direction 0,1,0
    Given I have a position 2,3,4 in the data
    And I create a scaling matrix A
    When I transform the ray with A
    Then the origin should be 2,6,12 a position
    And the direction should be 0,3,0 a vector
