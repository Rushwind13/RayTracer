# language: en
Feature: Tuples
    So that positions will work properly
    As a ray tracer
    I want to test many things about vector math

    Scenario: I can create positions
        Given I have a position 1,2,3 in the data
        Then It is a position
        Then It is not a vector

    Scenario: I can create vectors
        Given I have a vector 1,2,3 in the data
        Then It is a vector
        Then It is not a position

    Scenario: I can create colors
        Given I have a color 1,2,3 in the data
        Then It is a color
        Then It is not a position

    Scenario: add vector to position
        Given I have a position 1,0,0 in the data
        And I have a vector 0,1,0 in the data
        When I press add
        Then the result should be 1,1,0 a position
        And It is a position
        And The scale did not change

    Scenario: add two vectors
        Given I have a vector 1,0,0 in the data
        And I have a second vector 0,1,0 in the data
        When I press add_vector
        Then the result should be 1,1,0 a vector
        And It is a vector

    Scenario: subtract two positions
        Given I have a position 3,2,1 in the data
        And I have a second position 5,6,7 in the data
        When I press subtract_point
        Then the result should be -2,-4,-6 a vector
        And It is a vector

    Scenario: subtract vector from a position
        Given I have a position 3,2,1 in the data
        And I have a vector 5,6,7 in the data
        When I press subtract
        Then the result should be -2,-4,-6 a position
        And It is a position
        And The scale did not change

    Scenario: subtract two vectors
        Given I have a vector 3,2,1 in the data
        And I have a second vector 5,6,7 in the data
        When I press subtract_vector
        Then the result should be -2,-4,-6 a vector
        And It is a vector

    Scenario: subtract vector from the zero vector
        Given I have a vector 0,0,0 in the data
        And I have a second vector 1,-2,3 in the data
        When I press subtract_vector
        Then the result should be -1,2,-3 a vector
        And It is a vector

    Scenario: negate a vector
        Given I have a vector 1,-2,3 in the data
        When I press negate_vector
        Then the result should be -1,2,-3 a vector
        And It is a vector

    Scenario: multiply vector by a scalar
        Given I have a vector 1,-2,3 in the data
        When I press scale_vector 3.5
        Then the result should be 3.5,-7,10.5 a vector
        And It is a vector

    Scenario: multiply position by a scalar
        Given I have a position 1,-2,3 in the data
        When I press scale_position 3.5
        Then Results in 3.5,-7,10.5,3.5 a position
        And Result is a position
        And The scale changed to 3.5

    Scenario: multiply vector by a fractional scalar
        Given I have a vector 1,-2,3 in the data
        When I press scale_vector 0.5
        Then the result should be 0.5,-1,1.5 a vector
        And It is a vector

    Scenario: multiply position by a fractional scalar
        Given I have a position 1,-2,3 in the data
        When I press scale_position 0.5
        Then Results in 0.5,-1,1.5,0.5 a position
        And It is a position
        And The scale changed to 0.5

    Scenario: divide vector by a scalar
        Given I have a vector 1,-2,3 in the data
        When I press divide_vector 2
        Then the result should be 0.5,-1,1.5 a vector
        And It is a vector

    Scenario: divide position by a scalar
        Given I have a position 1,-2,3 in the data
        When I press divide_position 2
        Then Results in 0.5,-1,1.5,0.5 a position
        And It is a position
        And The scale changed to 0.5

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
        And It is a vector

    Scenario: magnitude of vector (1,2,3)
        Given I have a vector 1,2,3 in the data
        When I press normalize
        Then the result should be 0.26726,0.53452,0.80178 a vector
        Then result.x should be 0.26726
        Then result.y should be 0.53452
        Then result.z should be 0.80178
        And It is a vector

    Scenario: magnitude of a normalized vector
        Given I have a vector 1,2,3 in the data
        When I press magnorm
        Then the result should be 1.0 a float
        Then the result should be 0.26726,0.53452,0.80178 a vector
        And It is a vector

    Scenario: dot product of two vectors
        Given I have a vector 1,2,3 in the data
        And I have a second vector 2,3,4 in the data
        When I press dotprod
        Then the result should be 20 a float

    Scenario: dot product of two positions
        Given I have a position 1,2,3 in the data
        And I have a second position 2,3,4 in the data
        When I press dotprod_position
        Then the result should be 21 a float
        #And I'm not sure this is correct.

    Scenario: cross product of two vectors (axb)
        Given I have a vector 1,2,3 in the data
        And I have a second vector 2,3,4 in the data
        When I press crossprod
        Then the result should be -1,2,-1 a vector
        And It is a vector

    Scenario: cross product of two vectors (bxa)
        Given I have a vector 2,3,4 in the data
        And I have a second vector 1,2,3 in the data
        When I press crossprod
        Then the result should be 1,-2,1 a vector
        And It is a vector

    Scenario: reflect a vector around a normal
        Given I have a vector 1,1,0 in the data
        And I have a second vector 0,1,0 in the data
        When I press ReflectVector
        Then result.x should be -0.707107
        Then result.y should be 0.707107
        Then result.z should be 0
        Then the result should be -0.707107,0.707107,0 a vector
        And It is a vector
#### End of Chapter 1
    Scenario: Colors are (red, green, blue) vectors
        Given I have a vector -0.5,0.4,1.7 in the data
        When I press Color_vector
        Then result.r should be -0.5
        Then result.g should be 0.4
        Then result.b should be 1.7
        And It is a color

    Scenario: Colors are their own type
        Given I have a color -0.5,0.4,1.7 in the data
        When I press Color
        Then result.r should be -0.5
        Then result.g should be 0.4
        Then result.b should be 1.7
        And It is a color

    Scenario: add two colors
        Given I have a color 0.9,0.6,0.75 in the data
        And I have a second color 0.7,0.1,0.25 in the data
        When I press add_color
        Then the result should be 1.6,0.7,1.0 a color
        And It is a color

    Scenario: subtract two colors
        Given I have a color 0.9,0.6,0.75 in the data
        And I have a second color 0.7,0.1,0.25 in the data
        When I press subtract_color
        Then the result should be 0.2,0.5,0.5 a color
        And It is a color

    Scenario: multiply color by scalar
        Given I have a color 0.2,0.3,0.4 in the data
        When I press scale_color 2
        Then the result should be 0.4,0.6,0.8 a color
        And It is a color

    Scenario: multiply two colors
        Given I have a color 1,0.2,0.4 in the data
        And I have a second color 0.9,1,0.1 in the data
        When I press multiply_color
        Then the result should be 0.9,0.2,0.04 a color
        And It is a color
