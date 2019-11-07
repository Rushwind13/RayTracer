# language: en
Feature: Tuples
  So that positions will work properly
  As a ray tracer
  I want to test many things about vector math

    Scenario: add vector to position
      Given I have a position 1,0,0 in the data
      And I have a vector 0,1,0 in the data
      When I press add
      Then the result should be 1,1,0 a position

    Scenario: add two vectors
      Given I have a vector 1,0,0 in the data
      And I have a second vector 0,1,0 in the data
      When I press add_vector
      Then the result should be 1,1,0 a vector

    Scenario: subtract two positions
        Given I have a position 3,2,1 in the data
        And I have a second position 5,6,7 in the data
        When I press subtract_point
        Then the result should be -2,-4,-6 a vector

    Scenario: subtract vector from a position
        Given I have a position 3,2,1 in the data
        And I have a vector 5,6,7 in the data
        When I press subtract
        Then the result should be -2,-4,-6 a position

    Scenario: subtract two vectors
      Given I have a vector 3,2,1 in the data
      And I have a second vector 5,6,7 in the data
      When I press subtract_vector
      Then the result should be -2,-4,-6 a vector
