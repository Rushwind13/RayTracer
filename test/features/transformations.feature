# language: en
Feature: Matrix Transformations
  In order to completely change the renderer
  As a ray tracer
  I want to do lots of math about Matrix transformations
## Translation
  Scenario: Apply a translation matrix to a position
    Given I have a position 5,-3,2 in the data
    And I create a translation matrix
    When I transform the position -3,4,5 with A
    Then the result should be 2,1,7 a position
    And It is a position
    And The scale did not change

  Scenario: Apply the inverse of a translation matrix to a position
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

  Scenario: Translation does not affect vectors
    Given I have a position 5,-3,2 in the data
    And I create a translation matrix
    And I have a vector -3,4,5 in the data
    Then A * v = <-3,4,5> is a vector
    And It is a vector
## Scaling
  Scenario: Apply a scaling matrix to a position
    Given I have a position 2,3,4 in the data
    And I create a scaling matrix
    When I transform the position -4,6,8 with A
    Then the result should be -8,18,32 a position
    And It is a position
    And The scale did not change

  Scenario: Apply a scaling matrix to a vector
    Given I have a position 2,3,4 in the data
    And I create a scaling matrix
    When I transform the vector -4,6,8 with A
    Then the result should be -8,18,32 a vector
    And It is a vector

  Scenario: Apply the inverse of a scaling matrix to a position
    Given I have a position 2,4,5 in the data
    And I create a scaling matrix
    When I invert the matrix A
    When I transform the position -4,6,8 with R
    Then A-1 the inverse of A is the following matrix
      |  x  |  y  |  z  |  w   |
      | 0.5 | 0   | 0   | 0    |
      | 0   | 0.25| 0   | 0    |
      | 0   | 0   | 0.2 | 0    |
      | 0   | 0   | 0   | 1    |
    Then the result should be -2,1.5,1.6 a position
    And It is a position
    And The scale did not change

  Scenario: Reflection is scaling by a negative value
    Given I have a position -1,1,1 in the data
    And I create a scaling matrix
    When I transform the position 2,3,4 with A
    Then the result should be -2,3,4 a position
    And It is a position
    And The scale did not change
