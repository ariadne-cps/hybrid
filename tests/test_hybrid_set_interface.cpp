// Typed cloning/conversion contracts, including ownership on a failed cast.
#include <memory>
#include <sstream>
#include "ariadne-testing.hpp"
#include "hybrid/hybrid_set.hpp"
#include "hybrid/hybrid_bounded_constraint_set.hpp"
#include "hybrid/hybrid_constraint_set.hpp"

using namespace Ariadne;

// An exact unit box provides all validated regular/located views. Keeping
// this fixture independent of the effective hierarchy tests its own ABI.
class ValidatedUnitBox final : public ValidatedHybridSetInterface {
    DiscreteLocation _location;
    RealSpace _space;
    ExactBoxType _box;
public:
    ValidatedUnitBox(DiscreteLocation loc,RealSpace space)
        : _location(loc),_space(space),_box({{0,1}}) { }
    Set<RealVariable> variables(DiscreteLocation loc) const override {
        ARIADNE_PRECONDITION(loc==_location); return _space.variables();
    }
    Set<DiscreteLocation> locations() const override { return {_location}; }
    ValidatedLowerKleenean overlaps(const HybridExactBox& box) const override {
        if(box.location()!=_location) return false;
        return decide(_box.overlaps(box.euclidean_set(_space)));
    }
    ValidatedLowerKleenean separated(const HybridExactBox& box) const override {
        if(box.location()!=_location) return true;
        return decide(_box.separated(box.euclidean_set(_space)));
    }
    ValidatedLowerKleenean covers(const ExactBoxType& box) const override {
        return decide(_box.covers(box));
    }
    ValidatedLowerKleenean inside(const HybridExactBoxes& boxes) const override {
        if(!boxes.has_location(_location)) return false;
        return decide(_box.inside(boxes.euclidean_set(_location,_space)));
    }
    HybridUpperBoxes bounding_box() const override {
        HybridUpperBoxes result;
        result.insert(_location,_space,UpperBoxType(_box,dp)); return result;
    }
    OutputStream& _write(OutputStream& os) const override { return os<<_box; }
protected:
    HybridSetInterfaceBase* _clone() const override { return new ValidatedUnitBox(*this); }
    EuclideanSetInterfaceBase* _euclidean_set(DiscreteLocation,RealSpace) const override {
        return new ValidatedConstrainedImageSet(_box);
    }
};

class TrackedHybridConstraint final : public HybridConstraintSet {
    int& _destroyed;
public:
    explicit TrackedHybridConstraint(int& count) : _destroyed(count) { }
    ~TrackedHybridConstraint() override { ++_destroyed; }
};
class TrackedEuclideanConstraint final : public ConstraintSet {
    int& _destroyed;
public:
    TrackedEuclideanConstraint(const ConstraintSet& set,int& count)
        : ConstraintSet(set),_destroyed(count) { }
    ~TrackedEuclideanConstraint() override { ++_destroyed; }
};

// A third-party implementation can return an incompatible virtual result.
// The typed public facades must reject it and release its owned allocation.
class IncompatibleResults final : public HybridBoundedConstraintSet {
    ConstraintSet _wrong;
    int& _destroyed;
public:
    IncompatibleResults(DiscreteLocation loc,const RealExpressionBoundedConstraintSet& set,
                        const ConstraintSet& wrong,int& count)
        : HybridBoundedConstraintSet(loc,set),_wrong(wrong),_destroyed(count) { }
protected:
    HybridSetInterfaceBase* _clone() const override { return new TrackedHybridConstraint(_destroyed); }
    EuclideanSetInterfaceBase* _euclidean_set(DiscreteLocation,RealSpace) const override {
        return new TrackedEuclideanConstraint(_wrong,_destroyed);
    }
};

class TestHybridInterfaces : public Ariadne::Testing::ContextFixture {
public:
    using ContextFixture::ContextFixture;
    template<class Interface,class Object>
    Void check_clone(const Object& object,DiscreteLocation loc,const RealVariable& variable) {
        const Interface& view=object;
        std::unique_ptr<Interface> copy(view.clone());
        ARIADNE_TESTING_PROVE(copy.get()!=nullptr);
        ARIADNE_TESTING_PROVE(copy.get()!=&view);
        ARIADNE_TESTING_PROVE(copy->variables(loc).contains(variable));
    }
        Void test_effective_views() {
        DiscreteLocation loc(1); RealVariable x("x"); RealSpace space({x});
        HybridBoxSet box(loc,RealVariablesBox({0<=x<=1}));
        check_clone<EffectiveHybridBoundedSetInterface>(box,loc,x);
        check_clone<EffectiveHybridOvertSetInterface>(box,loc,x);
        check_clone<EffectiveHybridOpenSetInterface>(box,loc,x);
        check_clone<EffectiveHybridClosedSetInterface>(box,loc,x);
        check_clone<EffectiveHybridCompactSetInterface>(box,loc,x);
        check_clone<EffectiveHybridRegularSetInterface>(box,loc,x);
        check_clone<EffectiveHybridLocatedSetInterface>(box,loc,x);
        check_clone<EffectiveHybridSetInterface>(box,loc,x);
        const EffectiveHybridOpenSetInterface& open=box;
        const EffectiveHybridRegularSetInterface& regular=box;
        ARIADNE_TESTING_EXPECT_EQUAL(open.euclidean_set(loc,space).dimension(),1u);
        ARIADNE_TESTING_EXPECT_EQUAL(regular.euclidean_set(loc,space).dimension(),1u);
        ARIADNE_TESTING_PROVE(definitely(open.euclidean_set(loc,space).covers(ExactBoxType({{0.25_x,0.75_x}}))));
        std::ostringstream text; text<<static_cast<const HybridSetInterfaceBase&>(box);
        ARIADNE_TESTING_PROVE(!text.str().empty());
        HybridConstraintSet constraints(loc,RealExpressionConstraintSet({x>=0}));
        check_clone<EffectiveHybridClosedSetInterface>(constraints,loc,x);
        const HybridSetInterfaceBase& root=constraints;
        ARIADNE_TESTING_EXPECT_EQUAL(root.euclidean_set(loc,space).dimension(),1u);
        HybridBoundedConstraintSet bounded(loc,{0<=x<=1});
        check_clone<EffectiveHybridSetInterface>(bounded,loc,x);
    }
    Void test_validated_views() {
        DiscreteLocation loc(1); RealVariable x("x"); RealSpace space({x});
        ValidatedUnitBox box(loc,space);
        check_clone<ValidatedHybridBoundedSetInterface>(box,loc,x);
        check_clone<ValidatedHybridOvertSetInterface>(box,loc,x);
        check_clone<ValidatedHybridOpenSetInterface>(box,loc,x);
        check_clone<ValidatedHybridClosedSetInterface>(box,loc,x);
        check_clone<ValidatedHybridCompactSetInterface>(box,loc,x);
        check_clone<ValidatedHybridRegularSetInterface>(box,loc,x);
        check_clone<ValidatedHybridLocatedSetInterface>(box,loc,x);
        check_clone<ValidatedHybridSetInterface>(box,loc,x);
        HybridExactBox interior(loc,space,ExactBoxType({{0.25_x,0.75_x}}));
        ARIADNE_TESTING_PROVE(definitely(box.overlaps(interior)));
        ARIADNE_TESTING_PROVE(definitely(box.covers(interior.euclidean_set())));
        ARIADNE_TESTING_PROVE(box.bounding_box().has_location(loc));
    }
    Void test_failed_cast_ownership() {
        DiscreteLocation loc(1); RealVariable x("x"); RealSpace space({x});
        HybridConstraintSet constraints(loc,RealExpressionConstraintSet({x>=0}));
        int destroyed=0;
        IncompatibleResults faulty(loc,RealExpressionBoundedConstraintSet({0<=x<=1}),
                                   constraints.euclidean_set(loc,space),destroyed);
        const EffectiveHybridBoundedSetInterface& view=faulty;
        ARIADNE_TESTING_EXPECT_THROWS(std::bad_cast, std::unique_ptr<EffectiveHybridBoundedSetInterface>(view.clone()));
        ARIADNE_TESTING_EXPECT_EQUAL(destroyed,1);
        ARIADNE_TESTING_EXPECT_THROWS(std::bad_cast, static_cast<void>(view.euclidean_set(loc,space)));
        ARIADNE_TESTING_EXPECT_EQUAL(destroyed,2);
    }
    Void test_location_contracts() {
        DiscreteLocation loc(1),missing(2); RealVariable x("x"); RealSpace space({x});
        HybridConstraintSet constraints(loc,RealExpressionConstraintSet({x>=0}));
        ARIADNE_TESTING_EXPECT_EXCEPTION(constraints.variables(missing));
        ARIADNE_TESTING_EXPECT_EXCEPTION(constraints.continuous_set(missing));
        ARIADNE_TESTING_EXPECT_EXCEPTION(constraints.euclidean_set(missing,space));
        HybridBoundedConstraintSet bounded(loc,{0<=x<=1});
        ARIADNE_TESTING_EXPECT_EXCEPTION(bounded.variables(missing));
        ARIADNE_TESTING_EXPECT_EXCEPTION(bounded.continuous_set(missing));
        ARIADNE_TESTING_EXPECT_EXCEPTION(bounded.euclidean_set(missing,space));
        HybridBoundedConstraintSet empty;
        ARIADNE_TESTING_EXPECT_EXCEPTION(empty.location());
        bounded.adjoin(missing,RealExpressionBoundedConstraintSet({0<=x<=1}));
        ARIADNE_TESTING_EXPECT_EXCEPTION(bounded.location());
        HybridBoxSet box(loc,RealVariablesBox({0<=x<=1}));
        ARIADNE_TESTING_EXPECT_EXCEPTION(box.euclidean_set(missing,space));
        ARIADNE_TESTING_EXPECT_EXCEPTION(box.space(missing));
    }
};

int main(int argc, char** argv) {
    ARIADNE_TESTING_SUITE("test_hybrid_set_interface");
    ARIADNE_TESTING_ADD_TEST(TestHybridInterfaces, test_effective_views);
    ARIADNE_TESTING_ADD_TEST(TestHybridInterfaces, test_validated_views);
    ARIADNE_TESTING_ADD_TEST(TestHybridInterfaces, test_failed_cast_ownership);
    ARIADNE_TESTING_ADD_TEST(TestHybridInterfaces, test_location_contracts);
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc, argv);
}
