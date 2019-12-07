# language: en
Feature: Boxes
  So that I can use boxes in my scenes
  As a Ray Tracer
  I want to do lots of math with boxes

Scenario: A ray intersects a box +X
  Given I have a box
  And I have a Ray with origin 5,0.5,0 and direction -1,0,0
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6

Scenario: A ray intersects a box -X
  Given I have a box
  And I have a Ray with origin -5,0.5,0 and direction 1,0,0
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6

Scenario: A ray intersects a box +Y
  Given I have a box
  And I have a Ray with origin 0.5,5,0 and direction 0,-1,0
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6

Scenario: A ray intersects a box -Y
  Given I have a box
  And I have a Ray with origin 0.5,-5,0 and direction 0,1,0
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6

Scenario: A ray intersects a box +Z
  Given I have a box
  And I have a Ray with origin 0.5,0,5 and direction 0,0,-1
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6

Scenario: A ray intersects a box -Z
  Given I have a box
  And I have a Ray with origin 0.5,0,-5 and direction 0,0,1
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6

Scenario: A ray intersects a box inside
  Given I have a box
  And I have a Ray with origin 0,0.5,0 and direction 0,0,1
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6
