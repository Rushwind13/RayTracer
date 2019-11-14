# language: en
Feature: Matrix
  In order to understand Matrices better
  As a ray tracer
  I want to do lots of Matrix math

  Scenario: Constructing and inspecting a 4x4 matrix
    Given the following matrix M:
      |  x  |  y  |  z  |  w  |
      |  1  |  2  |  3  |  4  |
      |  5.5|  6.5|  7.5|  8.5|
      |  9  | 10  | 11  | 12  |
      | 13.5| 14.5| 15.5| 16.5|
    Then M4<0,0> = 1
    And M4<0,3> = 4
    And M4<1,0> = 5.5
    And M4<1,2> = 7.5
    And M4<3,0> = 13.5
    And M4<3,2> = 15.5

  Scenario: Constructing and inspecting a 2x2 matrix
    Given the following 2x2 matrix M:
      |  x  |  y  |
      |  -3 |  5  |
      |  1  | -2  |
    Then M2<0,0> = -3
    And M2<0,1> = 5
    And M2<1,0> = 1
    And M2<1,1> = -2

  Scenario: Constructing and inspecting a 3x3 matrix
    Given the following 3x3 matrix M:
      |  x  |  y  |  z  |
      |  3  |  5  |  9  |
      |  1  | -2  | -7  |
      |  0  |  1  |  1  |
    Then M3<0,0> = 3
    And M3<1,1> = -2
    And M3<2,2> = 1

  Scenario: equality with two identical matrices
    Given the following matrix A:
      |  x  |  y  |  z  |  w  |
      |  1  |  2  |  3  |  4  |
      |  5.5|  6.5|  7.5|  8.5|
      |  9  | 10  | 11  | 12  |
      | 13.5| 14.5| 15.5| 16.5|
    Given the following matrix B:
      |  x  |  y  |  z  |  w  |
      |  1  |  2  |  3  |  4  |
      |  5.5|  6.5|  7.5|  8.5|
      |  9  | 10  | 11  | 12  |
      | 13.5| 14.5| 15.5| 16.5|
    Then A = B

  Scenario: equality with two unequal matrices
    Given the following matrix A:
      |  x  |  y  |  z  |  w  |
      |  1  |  2  |  3  |  4  |
      |  5.5|  6.5|  7.5|  8.5|
      |  9  | 10  | 11  | 12  |
      | 13.5| 14.5| 15.5| 16.5|
    Given the following matrix B:
      |  x  |  y  |  z  |  w  |
      |  1  |  2  |  3  |  4  |
      |  5  |  6  |  7  |  8  |
      |  9  | 10  | 11  | 12  |
      | 13  | 14  | 15  | 16  |
    Then A != B

  Scenario: multiply two matrices
    Given the following matrix A:
      |  x  |  y  |  z  |  w  |
      |  1  |  2  |  3  |  4  |
      |  5  |  6  |  7  |  8  |
      |  9  |  8  |  7  |  6  |
      |  5  |  4  |  3  |  2  |
    Given the following matrix B:
      |  x  |  y  |  z  |  w  |
      | -2  |  1  |  2  |  3  |
      |  3  |  2  |  1  | -1  |
      |  4  |  3  |  6  |  5  |
      |  1  |  2  |  7  |  8  |
    #Then M4<0,0> = 0
    #Then M4<1,1> = 54
    #Then M4<2,2> = 110
    #Then M4<3,3> = 42
    Then A * B is the following 4x4 matrix:
      |  x  |  y  |  z  |  w  |
      | 20  | 22  | 50  | 48  |
      | 44  | 54  |114  |108  |
      | 40  | 58  |110  |102  |
      | 16  | 26  | 46  | 42  |

  Scenario: multiply matrix by position
    Given the following matrix A:
      |  x  |  y  |  z  |  w  |
      |  1  |  2  |  3  |  4  |
      |  2  |  4  |  4  |  2  |
      |  8  |  6  |  4  |  1  |
      |  0  |  0  |  0  |  1  |
    And I have a position <1,2,3,1> in the data
    Then A * b = <18,24,33,1> is a position

    Scenario: multiply matrix by identity matrix
      Given the following matrix A:
        |  x  |  y  |  z  |  w  |
        |  1  |  2  |  3  |  4  |
        |  2  |  4  |  4  |  2  |
        |  8  |  6  |  4  |  1  |
        |  0  |  0  |  0  |  1  |
      Then A * I = A

    Scenario: multiply position by identity matrix
        Given I have a position <1,2,3,1> in the data
        Then I * b = b

    Scenario: transpose matrix
      Given the following matrix A:
        |  x  |  y  |  z  |  w  |
        |  1  |  2  |  3  |  4  |
        |  2  |  4  |  4  |  2  |
        |  8  |  6  |  4  |  1  |
        |  0  |  0  |  0  |  1  |
      Then M transpose, MT, is the following matrix
        |  x  |  y  |  z  |  w  |
        |  1  |  2  |  8  |  0  |
        |  2  |  4  |  6  |  0  |
        |  3  |  4  |  4  |  0  |
        |  4  |  2  |  1  |  1  |

    Scenario: identity transpose is itself
       Then I = IT

   Scenario: invert trivial matrix
     Given the following matrix A:
       |  x  |  y  |  z  |  w  |
       |  4  |  0  |  0  | -2.5|
       |  0  |  4  |  0  |  0  |
       |  0  |  0  |  4  | -5  |
       |  0  |  0  |  0  |  1  |
     When I invert the matrix A
     Then A-1 the inverse of A is the following matrix
       |  x  |  y  |  z  |  w   |
       | 0.25| 0   | 0   | 0.625|
       | 0   | 0.25| 0   | 0    |
       | 0   | 0   | 0.25| 1.25 |
       | 0   | 0   | 0   | 1    |

   Scenario: invert matrix
     Given the following matrix A:
       |  x  |  y  |  z  |  w  |
       |  1  |  2  |  3  |  4  |
       |  2  |  4  |  4  |  2  |
       |  8  |  6  |  4  |  1  |
       |  0  |  0  |  0  |  1  |
     When I invert the matrix A
     Then A-1 the inverse of A is the following matrix
       |  x  |  y  |  z  |  w   |
       | 0.4 |-0.5 | 0.2 |-0.8  |
       |-1.2 | 1   |-0.1 | 2.9  |
       | 1   |-0.5 | 0   |-3    |
       | 0   | 0   | 0   | 1    |

   Scenario: a matrix multiplied by its inverse is the identity matrix
     Given the following matrix A:
       |  x  |  y  |  z  |  w  |
       |  1  |  2  |  3  |  4  |
       |  2  |  4  |  4  |  2  |
       |  8  |  6  |  4  |  1  |
       |  0  |  0  |  0  |  1  |
      Then A-1 * A = I
