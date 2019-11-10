# language: en
Feature: Matrix Transformations
  In order to completely change the renderer
  As a ray tracer
  I want to do lots of math about Matrix transformations

  Scenario: Multiply by a translation Matrix
    Given I have a position 5,-3,2 in the data
    And I create a translation matrix
    When I transform the position -3,4,5 with A
    Then the result should be 2,1,7 a position
    And It is a position
    And The scale did not change

  Scenario: Multiply by inverse of a translation Matrix
    Given I have a position 5,-3,2 in the data
    And I create a translation matrix
    When I invert the matrix A
    When I transform the position -3,4,5 with R
    Then A-1 the inverse of A is the following matrix
      |  x  |  y  |  z  |  w   |
      | 1   | 0   | 0   |-5    |
      | 0   | 1   | 0   | 3    |
      | 0   | 0   | 1   |-2    |
      | 0   | 0   | 0   | 1    |
    Then the result should be -8,7,3 a position
    And It is a position
    And The scale did not change
