/* Copyright (c) 2019. The SimGrid Team. All rights reserved.               */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "src/include/catch.hpp"
#include "src/kernel/lmm/maxmin.hpp"
#include "src/surf/surf_interface.hpp"
#include "xbt/log.h"

namespace lmm = simgrid::kernel::lmm;

TEST_CASE("kernel::lmm Single constraint systems", "[kernel-lmm-single-sys]")
{
  lmm::System* Sys = lmm::make_new_maxmin_system(false);

  SECTION("Variable weight")
  {
    /*
     * System under consideration:
     * 1\times\rho_1^{1} + 1\times\rho_2^{2} + 1\times\rho_3^{3} \le 10
     */

    lmm::Constraint* sys_cnst = Sys->constraint_new(nullptr, 10);
    lmm::Variable* sys_var_1  = Sys->variable_new(nullptr, 1, 0.0, 1);
    lmm::Variable* sys_var_2  = Sys->variable_new(nullptr, 2, 0.0, 1);
    lmm::Variable* sys_var_3  = Sys->variable_new(nullptr, 3, 0.0, 1);

    Sys->expand(sys_cnst, sys_var_1, 1);
    Sys->expand(sys_cnst, sys_var_2, 1);
    Sys->expand(sys_cnst, sys_var_3, 1);
    Sys->solve();

    REQUIRE(double_equals(sys_var_1->get_value(), 5.45455, sg_maxmin_precision));
    REQUIRE(double_equals(sys_var_2->get_value(), 2.72727, sg_maxmin_precision));
    REQUIRE(double_equals(sys_var_3->get_value(), 1.81818, sg_maxmin_precision));
  }

  SECTION("Consumption weight")
  {
    /*
     * System under consideration:
     * 1\times\rho_1^{1} + 2\times\rho_2^{1} + 3\times\rho_3^{1} \le 10
     */

    lmm::Constraint* sys_cnst = Sys->constraint_new(nullptr, 10);
    lmm::Variable* sys_var_1  = Sys->variable_new(nullptr, 1, 0.0, 1);
    lmm::Variable* sys_var_2  = Sys->variable_new(nullptr, 1, 0.0, 1);
    lmm::Variable* sys_var_3  = Sys->variable_new(nullptr, 1, 0.0, 1);

    Sys->expand(sys_cnst, sys_var_1, 1);
    Sys->expand(sys_cnst, sys_var_2, 2);
    Sys->expand(sys_cnst, sys_var_3, 3);
    Sys->solve();

    REQUIRE(double_equals(sys_var_1->get_value(), 1.666667, sg_maxmin_precision));
    REQUIRE(double_equals(sys_var_2->get_value(), 1.666667, sg_maxmin_precision));
    REQUIRE(double_equals(sys_var_3->get_value(), 1.666667, sg_maxmin_precision));
  }

  SECTION("Consumption weight + variable weight")
  {
    /*
     * Strange system under consideration:
     * 56\times\rho_1^{74} + 21\times\rho_2^{6} + 2\times\rho_3^{2} \le 123
     */

    lmm::Constraint* sys_cnst = Sys->constraint_new(nullptr, 123);
    lmm::Variable* sys_var_1  = Sys->variable_new(nullptr, 56, 0.0, 1);
    lmm::Variable* sys_var_2  = Sys->variable_new(nullptr, 21, 0.0, 1);
    lmm::Variable* sys_var_3  = Sys->variable_new(nullptr, 3, 0.0, 1);

    Sys->expand(sys_cnst, sys_var_1, 74);
    Sys->expand(sys_cnst, sys_var_2, 6);
    Sys->expand(sys_cnst, sys_var_3, 2);
    Sys->solve();

    REQUIRE(double_equals(sys_var_1->get_value(), 0.9659686, sg_maxmin_precision));
    REQUIRE(double_equals(sys_var_2->get_value(), 2.575916, sg_maxmin_precision));
    REQUIRE(double_equals(sys_var_3->get_value(), 18.03141, sg_maxmin_precision));
  }
}

TEST_CASE("kernel::lmm Multiple constraint systems", "[kernel-lmm-multiple-sys]")
{

  lmm::System* Sys = lmm::make_new_maxmin_system(false);

  SECTION("3 Constraints system")
  {

    /*
     * System under consideration:
     * 4\times\rho_1^{5.1} + 2.6\times\rho_2^{7} + 1.2\times\rho_3^{8.5} \le 14.6 \\
     * 5\times\rho_4^{6.2} + 2\times\rho_2^{7}   + 4.1\times\rho_3^{8.5} \le 40.7 \\
     * 6\times\rho_5^1                                                   \le 7
     */

    double test_accuracy = 0.001; // Don't go to far with the accuracy

    lmm::Constraint* sys_cnst_1 = Sys->constraint_new(nullptr, 14.6);
    lmm::Constraint* sys_cnst_2 = Sys->constraint_new(nullptr, 10.7);
    lmm::Constraint* sys_cnst_3 = Sys->constraint_new(nullptr, 7);

    lmm::Variable* sys_var_1 = Sys->variable_new(nullptr, 5.1, 0.0, 1);
    lmm::Variable* sys_var_2 = Sys->variable_new(nullptr, 7, 0.0, 2);
    lmm::Variable* sys_var_3 = Sys->variable_new(nullptr, 8.5, 0.0, 2);
    lmm::Variable* sys_var_4 = Sys->variable_new(nullptr, 6.2, 0.0, 1);
    lmm::Variable* sys_var_5 = Sys->variable_new(nullptr, 1, 0.0, 1);

    // Constraint 1
    Sys->expand(sys_cnst_1, sys_var_1, 4);
    Sys->expand(sys_cnst_1, sys_var_2, 2.6);
    Sys->expand(sys_cnst_1, sys_var_3, 1.2);
    // Constraint 2
    Sys->expand(sys_cnst_2, sys_var_4, 5);
    Sys->expand(sys_cnst_2, sys_var_2, 2);
    Sys->expand(sys_cnst_2, sys_var_3, 4.1);
    // Constraint 3
    Sys->expand(sys_cnst_3, sys_var_5, 6);
    Sys->solve();

    REQUIRE(double_equals(sys_var_1->get_value(), 2.779119, test_accuracy));
    REQUIRE(double_equals(sys_var_2->get_value(), 0.9708181, test_accuracy));
    REQUIRE(double_equals(sys_var_3->get_value(), 0.7994973, test_accuracy));
    REQUIRE(double_equals(sys_var_4->get_value(), 1.096085, test_accuracy));
    REQUIRE(double_equals(sys_var_5->get_value(), 1.166667, test_accuracy));
  }
}