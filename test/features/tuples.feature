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

    Scenario: subtract vector from the zero vector
        Given I have a vector 0,0,0 in the data
        And I have a second vector 1,-2,3 in the data
        When I press subtract_vector
        Then the result should be -1,2,-3 a vector

    Scenario: negate a vector
        Given I have a vector 1,-2,3 in the data
        When I press negate_vector
        Then the result should be -1,2,-3 a vector

    Scenario: multiply vector by a scalar
        Given I have a vector 1,-2,3 in the data
        When I press scale_vector 3.5
        Then the result should be 3.5,-7,10.5 a vector

    Scenario: multiply position by a scalar
        Given I have a position 1,-2,3 in the data
        When I press scale_position 3.5
        Then the result should be 3.5,-7,10.5 a position

    Scenario: multiply vector by a fractional scalar
        Given I have a vector 1,-2,3 in the data
        When I press scale_vector 0.5
        Then the result should be 0.5,-1,1.5 a vector

    Scenario: multiply position by a fractional scalar
        Given I have a position 1,-2,3 in the data
        When I press scale_position 0.5
        Then the result should be 0.5,-1,1.5 a position

    Scenario: divide vector by a scalar
        Given I have a vector 1,-2,3 in the data
        When I press divide_vector 2
        Then the result should be 0.5,-1,1.5 a vector

    Scenario: divide position by a scalar
        Given I have a position 1,-2,3 in the data
        When I press divide_position 2
        Then the result should be 0.5,-1,1.5 a position

    Scenario: magnitude of vector (1,0,0)
        Given I have a vector 1,0,0 in the data
        When I press magnitude
        Then the result should be 1.0 a float

    Scenario: magnitude of vector (0,1,0)
        Given I have a vector 0,1,0 in the data
        When I press magnitude
        Then the result should be 1.0 a float

    Scenario: magnitude of vector (0,0,1)
        Given I have a vector 0,0,1 in the data
        When I press magnitude
        Then the result should be 1.0 a float

    Scenario: magnitude of vector (1,2,3)
        Given I have a vector 1,2,3 in the data
        When I press magnitude
        Then the result should be 3.74166 a float

    Scenario: magnitude of vector (-1,-2,-3)
        Given I have a vector -1,-2,-3 in the data
        When I press magnitude
        Then the result should be 3.74166 a float

    Scenario: Normalizing vector (4,0,0) gives (1,0,0)
        Given I have a vector 4,0,0 in the data
        When I press normalize
        Then the result should be 1,0,0 a vector

    Scenario: magnitude of vector (1,2,3)
        Given I have a vector 1,2,3 in the data
        When I press normalize
        Then the result should be 0.26726,0.53452,0.80178 a vector
        Then result.x should be 0.26726
        Then result.y should be 0.53452
        Then result.z should be 0.80178

    Scenario: magnitude of a normalized vector
        Given I have a vector 1,2,3 in the data
        When I press magnorm
        Then the result should be 1.0 a float
        Then the result should be 0.26726,0.53452,0.80178 a vector
