namespace ChiLua
 {
int chiTextSetProperty(int handle, int property, varying value);
int chiTextCreate(char* name);
int chi3DLineSetTransform(int index, int transformIndex);
int chi3DLineSetviewport(int index, int viewportIndex);
int chi3DLineChangeColor(int index, float r, float g, float b, float a);
int chi3DLineAddVertex(int index, float x, float y, float z);
int chi3DLineChangeVertex(int index, int vertNum, float x, float y, float z);
int chi3DLineSetStipple(int index, bool stippleFlag, int stippleFactor, float lineWidth);
int chi3DLineCreate(char* name);
int chi3DLineCreateFromLineMesh(char* name);
int chiLightSetProperty();
int chiLightCreate();
int chiShadowsEnable();
int chiShadowsDisable();
int chiLineAddVertex(int index, float x, float y, float z);
int chiLineChangeColor(int index, float r, float g, float b, float a);
int chiLineSetStipple(int index, bool stippleFlag, int stippleFactor, float lineWidth);
int chiLineSetviewport(int index, int viewportIndex);
int chiLineCreate(char* name);
int chiLineChangeVertex(int index, int vertNum, float x, float y, float z);
int chiObjectCreate();
int chiObjectExportSurface(int SurfaceHandle);
int chiObjectLoadSurface();
int chiObjectLoadSurfaceFromSurfaceMesh(int Handle);
int chiObjectAddSurface();
int chiObjectQuery();
int chiObjectSetProperty();
int chiObjectGetCentroid();
int chiTextureLoad();
int chiSleep(int time);
int chiDisplayerCreate();
int chiDisplayerSetViewport(double xmin, double ymin, double xmax, double ymax);
int chiDisplayerGetViewport();
int chiSet2D(bool flag);
int chiSet3D(bool flag);
int chiBindScene(int number);
int chiGetScene();
int chiGetSceneCount();
int chiCreateScene();
int chiRequestSceneRefresh(int sceneNumber);
int chiSetSceneUpdateMode(int sceneMode);
int chiViewportSetProperty(double xmin, double ymin, double xmax, double ymax);
int chiViewportCreate(str name);
int chiGraphicsOrientCamera();
int chiGraphicsCameraType();
int chiGraphicsRotateCamera();
int chiGraphicsGetCameraRotation();
int chiGraphicsTranslateCamera();
int chiGraphicsCameraOrthoWidth(float orthoWidth, bool orthoRound=false);
int chiGraphicsCameraFOV();
int chiGraphicsPositionCamera();
int chiGraphicsGetCameraPosition();
int chiWindowCreate(char name, bool defer);
int chiSetWindowProperties();
int chiSetWindowScene();
int chiGetWindowProperties();
int chiWindowMaximize();
int chiWindowNormalize();
int chiWindowSetCursor();
int chiSetLabel();
int chiSetLabelProperty();
int chiTransformQuery(int queryNumber);
int chiTransformGet(int transformIndex);
int chiTransformCreate(char* name);
int chiTransformSetScale(int transformHandle, double dx, double dy, double dz);
int chiTransformSetRotationPoint(int transformHandle, double dx, double dy, double dz);
int chiTransformSetRotation(int transformHandle, double dx, double dy, double dz);
int chiTransformSetParent(int childIndex, int parentIndex);
int chiTransformSetTranslation(int transformHandle, double dx, double dy, double dz);
int chiMaterialQuery();
int chiMaterialGeneratePreview();
int chiMaterialCreate();
int chiMaterialSetProperty();
int chiMaterialUpdate();
int chi3DPointToScreen();
int chiPointCreate();
int chiPointAddVertex();
int chiPointChangeVertex();
int chiScreenTo3DPoint();
int chi3DPointCreate();
int chi3DPointAddVertex();
int chi3DPointChangeVertex();
int chi3DPointSetTransform();
int chiReloadShader();
int chiShaderQuery();
int chiPieExportPin(int pinNumber);
int chiPieSetPinMode(int pinNumber, int mode);
int chiPieSetPinValue(int pinNumber, int mode);
int chiPieGetPinValue(int pinNumber);
int chiPieInitSPI();
int chiPieReadSPIChannel(int channelNumber);
int chiPieSetSPIBuffer(int channelNumber, bool bufferFlag);
int chiPieGetSPIBuffer(int channelNumber, int bufferPos);
int chiPieInitializeSerial(int baudrate);
int chiPieSerialWrite(char message);
int chiPieSerialRead();
int chiThermoSetComponentProperty(Handle sysHndle, Handle compHndle, Property propCode);
int chiThermoGetComponentProperty(Handle sysHndle, Handle compHndle, Property propCode);
int chiThermoCreateSystem();
int chiThermoConnectTwoComponents(Handle systemHandle, Component leftComponent, Single sjunc, Component rigtComponent, 0=end-begin, mode);
int chiThermoInitialize(int systemHandle);
int chiThermoCreateVolumeFromCoordinates(int systemHandle, Table point1, Table point2);
int chiThermoCreateSJunction(Handle systemHandle);
int chiThermoCreateBC(int systemHandle);
int chiEdgeLoopSplitByAngle(int LoopCollectionHandle, int LoopHandle, double Angle);
int chiSurfaceMeshImportFromOBJFile();
int chiSurfaceMeshGetEdgeLoops();
int chiSurfaceMeshCreate();
int chiRegionAddSurfaceBoundary(int RegionHandle, int SurfaceHandle);
int chiRegionAddLineBoundary(int RegionHandle, int LineMeshHandle);
int chiRegionCreate();
int chiMeshHandlerCreate();
int chiLineMeshCreateFromLoop(int LoopCollectionHandle, int LoopHandle);
int chiSurfaceMesherCreate(int Type);
int chiSurfaceMesherExecute();
int chiVolumeMesherCreate(int Type);
int chiVolumeMesherExecute();
int chiSolverExecute();
int chiSolverAddRegion(int SolverHandle, int RegionHandle);
int chiMonteCarlonCreateSource(int SolverHandle);
int chiMonteCarlonCreateSolver();
int chiDiffusionExecute(int SolverHandle);
int chiDiffusionCreateSolver();
int chiDiffusionInitialize(int SolverHandle);
int chiDiffusionSetProperty(int SolverHandle, int PropertyIndex, varying Values);
}