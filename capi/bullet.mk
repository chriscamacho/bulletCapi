# root of bullet source code
BULL=../../bullet3/src/

# sources required from each bullet component
CollisionShapesSRC=$(BULL)BulletCollision/CollisionShapes/btConcaveShape.cpp \
		$(BULL)BulletCollision/CollisionShapes/btOptimizedBvh.cpp \
		$(BULL)BulletCollision/CollisionShapes/btTriangleMeshShape.cpp \
		$(BULL)BulletCollision/CollisionShapes/btConvexPolyhedron.cpp \
		$(BULL)BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp \
		$(BULL)BulletCollision/CollisionShapes/btTriangleCallback.cpp \
		$(BULL)BulletCollision/CollisionShapes/btCollisionShape.cpp \
		$(BULL)BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp \
		$(BULL)BulletCollision/CollisionShapes/btSphereShape.cpp \
		$(BULL)BulletCollision/CollisionShapes/btConvexShape.cpp \
		$(BULL)BulletCollision/CollisionShapes/btConvexInternalShape.cpp \
		$(BULL)BulletCollision/CollisionShapes/btBoxShape.cpp \
		$(BULL)BulletCollision/CollisionShapes/btCylinderShape.cpp \
		$(BULL)BulletCollision/CollisionShapes/btCompoundShape.cpp
		
CollisionDispatchSRC=$(BULL)BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp \
		$(BULL)BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btHashedSimplePairCache.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btUnionFind.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btCollisionObject.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btCollisionWorld.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btManifoldResult.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp \
		$(BULL)BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp
		
BroadphaseCollisionSRC=$(BULL)BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp \
		$(BULL)BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp \
		$(BULL)BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp \
		$(BULL)BulletCollision/BroadphaseCollision/btDbvt.cpp \
		$(BULL)BulletCollision/BroadphaseCollision/btDispatcher.cpp \
		$(BULL)BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp
		
NarrowPhaseCollisionSRC=$(BULL)BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp \
		$(BULL)BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp \
		$(BULL)BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp \
		$(BULL)BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp \
		$(BULL)BulletCollision/NarrowPhaseCollision/btConvexCast.cpp \
		$(BULL)BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp \
		$(BULL)BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp \
		$(BULL)BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp \
		$(BULL)BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp \
		$(BULL)BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp \
		$(BULL)BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp \
		$(BULL)BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp
		
DynamicsSRC=$(BULL)BulletDynamics/Dynamics/btRigidBody.cpp \
		$(BULL)BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp
		
ConstraintSolverSRC=$(BULL)BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp \
		$(BULL)BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp \
		$(BULL)BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp \
		$(BULL)BulletDynamics/ConstraintSolver/btTypedConstraint.cpp

LinearMathSRC=$(BULL)LinearMath/btConvexHullComputer.cpp \
		$(BULL)LinearMath/btGeometryUtil.cpp \
		$(BULL)LinearMath/btQuickprof.cpp \
		$(BULL)LinearMath/btAlignedAllocator.cpp
		
# list .o files for each bullet component
LinearMathOBJ=$(patsubst $(BULL)LinearMath/%.cpp,obj/%.o,$(LinearMathSRC))
ConstraintSolverOBJ=$(patsubst $(BULL)BulletDynamics/ConstraintSolver/%.cpp,obj/%.o,$(ConstraintSolverSRC))
DynamicsOBJ=$(patsubst $(BULL)BulletDynamics/Dynamics/%.cpp,obj/%.o,$(DynamicsSRC))
NarrowPhaseCollisionOBJ=$(patsubst $(BULL)BulletCollision/NarrowPhaseCollision/%.cpp,obj/%.o,$(NarrowPhaseCollisionSRC))
BroadphaseCollisionOBJ=$(patsubst $(BULL)BulletCollision/BroadphaseCollision/%.cpp,obj/%.o,$(BroadphaseCollisionSRC))
CollisionDispatchOBJ=$(patsubst $(BULL)BulletCollision/CollisionDispatch/%.cpp,obj/%.o,$(CollisionDispatchSRC))
CollisionShapesOBJ=$(patsubst $(BULL)BulletCollision/CollisionShapes/%.cpp,obj/%.o,$(CollisionShapesSRC))

# all the bullet components together (object files)		
BOBJ=$(LinearMathOBJ) $(ConstraintSolverOBJ) $(DynamicsOBJ) \
	$(NarrowPhaseCollisionOBJ) $(BroadphaseCollisionOBJ) \
	$(CollisionDispatchOBJ) $(CollisionShapesOBJ) $(SharedMemoryOBJ) 


# compile rule for each component of bullet
$(LinearMathOBJ):	obj/%.o: $(BULL)LinearMath/%.cpp
	g++ $(FLAGS) -o $@ $^

$(ConstraintSolverOBJ):	obj/%.o: $(BULL)BulletDynamics/ConstraintSolver/%.cpp
	g++ $(FLAGS) -o $@ $^

$(DynamicsOBJ):	obj/%.o: $(BULL)BulletDynamics/Dynamics/%.cpp
	g++ $(FLAGS) -o $@ $^

$(NarrowPhaseCollisionOBJ):	obj/%.o: $(BULL)BulletCollision/NarrowPhaseCollision/%.cpp
	g++ $(FLAGS) -o $@ $^

$(BroadphaseCollisionOBJ):	obj/%.o: $(BULL)BulletCollision/BroadphaseCollision/%.cpp
	g++ $(FLAGS) -o $@ $^

$(CollisionDispatchOBJ):	obj/%.o: $(BULL)BulletCollision/CollisionDispatch/%.cpp
	g++ $(FLAGS) -o $@ $^

$(CollisionShapesOBJ):	obj/%.o: $(BULL)BulletCollision/CollisionShapes/%.cpp
	g++ $(FLAGS) -o $@ $^

lib/libbullet.a: $(BOBJ)
	ar -cvq lib/libbullet.a $(BOBJ)

