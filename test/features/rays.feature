# language: en
Feature: Rays
    In order to better understand rays
    As a ray tracer
    I want to do lots of math with Rays

    Scenario: create a Ray
        Given the following position 0,0,0 = origin
        And the following vector 0,0,-1 = direction
        When I press Ray
        Then the origin should be 0,0,0 a position
        Then the direction should be 0,0,-1 a vector
        Then the length should be 1.0 a float
