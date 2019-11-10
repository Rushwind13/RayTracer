# language: en
Feature: Matrix Transformations
  In order to completely change the renderer
  As a ray tracer
  I want to do lots of math about Matrix transformations

  Scenario: Multiply by a translation Matrix
  Given I have a position 5,-3,2 in the data
  And I create a translation matrix
  When I transform the position -3,4,5
  Then A * b = <2,1,7,1> is a position
  And It is a position
  And The scale did not change
