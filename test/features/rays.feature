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
