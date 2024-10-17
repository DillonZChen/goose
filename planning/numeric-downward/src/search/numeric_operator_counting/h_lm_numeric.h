#ifndef LM_NUMERIC_CONSTRAINTS_H
#define LM_NUMERIC_CONSTRAINTS_H

#include "../operator_counting/constraint_generator.h"
#include "../numeric_landmarks/landmark_factory_scala.h"
#include "../numeric_operator_counting/numeric_helper.h"

#include <memory>
#include <vector>

namespace options {
    class Options;
}

namespace numeric_helper {
    class NumericTaskProxy;
}
namespace operator_counting {
    class LMNumericConstriants : public ConstraintGenerator {
    protected:
        std::unique_ptr<landmarks::LandmarkFactoryScala> factory;
        numeric_helper::NumericTaskProxy numeric_task;
        
        // copy from delete task
        // TODO: move these in landmark factory
        //
        void build_first_achiever(vector<set<int>> &landmarks_table);
        std::vector<vector<bool>> fadd; // first time it is added: first index action, second index condition, value: true or false
        std::vector<set<int>> first_achievers; // first index condition, value: set of actions
        std::vector<vector<bool>> action_landmarks; // first index action, value: set of fact landmarks
        ///
        vector<lp::LPConstraint> constraints;

    public:
        
        virtual void initialize_constraints(const std::shared_ptr<AbstractTask> task,
                                            std::vector<lp::LPConstraint> &constraints,
                                            double infinity);
        virtual bool update_constraints(const State &state, lp::LPSolver &lp_solver);
    };
}

#endif
