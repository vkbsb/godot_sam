/* sumator.cpp */

#include "SuperAnimNode.h"
#include "SuperAnimSprite.h"

#include "core/io/file_access_buffered.h"
#include "core/io/resource_loader.h"
#include "core/os/os.h"

/*------------- FUNCTIONS FOR SUPER ANIM ----------------*/
// Operator between matrix & vertex
inline ccVertex3F operator*(const SuperAnim::SuperAnimMatrix3 &theMatrix3, const ccVertex3F &theVec)
{
    return vertex3(
                   theMatrix3.m00*theVec.x + theMatrix3.m01*theVec.y + theMatrix3.m02,
                   theMatrix3.m10*theVec.x + theMatrix3.m11*theVec.y + theMatrix3.m12,
                   theVec.z);
}

inline ccV3F_C4B_T2F_Quad operator*(const SuperAnim::SuperAnimMatrix3 &theMatrix3, const ccV3F_C4B_T2F_Quad &theQuad)
{
    ccV3F_C4B_T2F_Quad aNewQuad = theQuad;
    aNewQuad.bl.vertices = theMatrix3 * theQuad.bl.vertices;
    aNewQuad.br.vertices = theMatrix3 * theQuad.br.vertices;
    aNewQuad.tl.vertices = theMatrix3 * theQuad.tl.vertices;
    aNewQuad.tr.vertices = theMatrix3 * theQuad.tr.vertices;
    return aNewQuad;
}

/*--------------------------------------------------------*/


void SuperAnimNode::Pause()
{
    mAnimState = kAnimStatePause;
}

void SuperAnimNode::Resume()
{
    mAnimState = kAnimStatePlaying;
}

bool SuperAnimNode::IsPause(){
    return mAnimState == kAnimStatePause;
}

bool SuperAnimNode::IsPlaying(){
    return mAnimState == kAnimStatePlaying;
}

int SuperAnimNode::GetCurFrame(){
    return (int)mCurFrameNum;
}

int SuperAnimNode::GetId(){
    return mId;
}

void SuperAnimNode::SetCurSectionName(String section)
{
    PlaySection(section, true);
}

String SuperAnimNode::GetCurSectionName(){
    return mCurLabel;
}

float SuperAnimNode::get_width(){
    return this->mWidth;
}

float SuperAnimNode::get_height(){
    return this->mHeight;
}

void SuperAnimNode::_bind_methods() {

    ObjectTypeDB::bind_method("get_height", &SuperAnimNode::get_height);
    ObjectTypeDB::bind_method("get_width", &SuperAnimNode::get_width);
    ObjectTypeDB::bind_method("load_anim", &SuperAnimNode::load_anim);
    ObjectTypeDB::bind_method("play_section", &SuperAnimNode::PlaySection);
    ObjectTypeDB::bind_method("pause", &SuperAnimNode::Pause);
    ObjectTypeDB::bind_method("resume", &SuperAnimNode::Resume);
    ObjectTypeDB::bind_method("has_section", &SuperAnimNode::HasSection);


    ADD_SIGNAL(MethodInfo("AnimTimeEvent", PropertyInfo(Variant::STRING, "label"), PropertyInfo(Variant::INT, "mCurFrameNum"), PropertyInfo(Variant::REAL, "timeFactor")));
    ADD_SIGNAL(MethodInfo("AnimSectionEnd", PropertyInfo(Variant::STRING, "label")));

    //add boolean which lets you show stage borders.
    ObjectTypeDB::bind_method("set_showstage", &SuperAnimNode::set_showstage);
    ObjectTypeDB::bind_method("is_showstage", &SuperAnimNode::is_showstage);
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "showstage"), _SCS("set_showstage"),_SCS("is_showstage"));


    //add boolean which lets you show axis.
    ObjectTypeDB::bind_method("set_showaxis", &SuperAnimNode::set_showaxis);
    ObjectTypeDB::bind_method("is_showaxis", &SuperAnimNode::is_showaxis);
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "showaxis"), _SCS("set_showaxis"),_SCS("is_showaxis"));


    //ui element for loading the sam file from editor.
    ObjectTypeDB::bind_method("set_section", &SuperAnimNode::SetCurSectionName);
    ObjectTypeDB::bind_method("get_section", &SuperAnimNode::GetCurSectionName);
    ADD_PROPERTY( PropertyInfo( Variant::STRING, "CurSection"), _SCS("set_section"), _SCS("get_section"));

    //ui element for loading the sam file from editor.
    ObjectTypeDB::bind_method("set_sam", &SuperAnimNode::set_sam);
    ObjectTypeDB::bind_method("get_sam", &SuperAnimNode::get_sam);
    ADD_PROPERTY( PropertyInfo( Variant::OBJECT, "SuperAnim", PROPERTY_HINT_RESOURCE_TYPE,"SuperAnim"), _SCS("set_sam"), _SCS("get_sam"));
}

SuperAnimNode::SuperAnimNode() {
    print_line("+SuperAnimNode::SuperAnimNode()");
    count=0;
    myColor = Color(1, 0, 0, 1);
    
    print_line("Constructor");
    
    set_process(true);

    mAnimState = kAnimStateInvalid;
    mIsFlipX = mIsFlipY = false;
    mSpeedFactor = 1.0f;
    mIsLoop = false;
    mCurFrameNum = 0;

    p_points.resize(4);
    p_uvs.resize(4);

    showStage = true;

    print_line("-SuperAnimNode::SuperAnimNode()");
}

void SuperAnimNode::set_showaxis(bool p_ShowAxis)
{
    showAxis = p_ShowAxis;
}

bool SuperAnimNode::is_showaxis() const
{
    return showAxis;
}

void SuperAnimNode::set_showstage(bool p_ShowStage)
{
    showStage = p_ShowStage;
}

bool SuperAnimNode::is_showstage() const
{
    return showStage;
}

void SuperAnimNode::set_sam(const Ref<SuperAnimData> samRes)
{
    mSamRes = samRes;

    //we merge the details of SuerAnimDef into Node.
    if( !mSamRes.is_null() ){
        SuperAnimData *aMainDef = mSamRes.ptr();
        this->mMainDefKey = aMainDef->get_path();
        this->mAnimRate = aMainDef->mAnimRate;
        this->mWidth = aMainDef->mWidth;
        this->mHeight = aMainDef->mHeight;
        this->mCurFrameNum = aMainDef->mStartFrameNum;
        this->mIsHandlerValid = true;
        mAnimState = kAnimStateInitialized;

    } else {
        this->mIsHandlerValid = false;
    }
}

Ref<SuperAnimData> SuperAnimNode::get_sam() const{

    return mSamRes;
}

void SuperAnimNode::load_anim(String resPath)
{
    mSamRes = ResourceLoader::load(resPath);

//    set_sam(samRes);
    //we merge the details of SuerAnimDef into Node.
    if( !mSamRes.is_null() ){
        SuperAnimData *aMainDef = mSamRes.ptr();
        this->mMainDefKey = aMainDef->get_path();
        this->mAnimRate = aMainDef->mAnimRate;
        this->mWidth = aMainDef->mWidth;
        this->mHeight = aMainDef->mHeight;
        this->mCurFrameNum = aMainDef->mStartFrameNum;
        this->mIsHandlerValid = true;
        mAnimState = kAnimStateInitialized;

//        SuperAnim::SuperAnimSpriteMgr::GetInstance()->dumpSpritesInfo();
    } else {
        this->mIsHandlerValid = false;
    }
}

void SuperAnimNode::superAnimUpdate(float dt)
{
    if (mAnimState != kAnimStatePlaying)
    {
        return;
    }

    bool isNewLabel = false;
    float anOriginFrameRate = mAnimRate;
    mAnimRate *= mSpeedFactor;

    SuperAnim::IncAnimFrameNum(this, dt, isNewLabel);
    mAnimRate = anOriginFrameRate;

    float aTimeFactor = (mCurFrameNum - mFirstFrameNumOfCurLabel) / (float)(mLastFrameNumOfCurLabel - mFirstFrameNumOfCurLabel);
    emit_signal("AnimTimeEvent", mCurLabel, mCurFrameNum, aTimeFactor);


    if (isNewLabel && mIsLoop) {
        PlaySection(mCurLabel, mIsLoop);
    }

    if(isNewLabel){
        emit_signal("AnimSectionEnd", mCurLabel);
    }
}


bool SuperAnimNode::HasSection(const String &theLabelName){
    return  SuperAnim::HasSection((*this), theLabelName);
}

void SuperAnimNode::setSpeedFactor(float theNewSpeedFactor){
    mSpeedFactor = theNewSpeedFactor;
}

bool SuperAnimNode::PlaySection(const String &theLabel, bool isLoop)
{
    MYPRINT("+Node:PlaySection \n");
    if (mAnimState == kAnimStateInvalid)
    {
//        CCAssert(false, "The animation isn't ready.");
        print_line("The animation isn't ready.");
        return false;
    }

    if (theLabel.empty())
    {
//        CCAssert(false, "Please specify an animation section label to play.");
        print_line("Please specify an animation section label to play.");
        return false;
    }

    SuperAnim::SuperAnimMainDef *aMainDef =  SuperAnim::SuperAnimDefMgr::GetInstance()->Load_GetSuperAnimMainDef(mMainDefKey);
    if (aMainDef == NULL) {
        OS::get_singleton()->print("MainDef: %p", aMainDef);
        mIsHandlerValid = false;
        return false;
    }


    for(int i = 0; i < aMainDef->mLabels.size(); i++){
        if(aMainDef->mLabels[i].mLabelName == theLabel){
            SuperAnim::SuperAnimLabel it = aMainDef->mLabels[i];
            mCurFrameNum = it.mStartFrameNum;
            mCurLabel = theLabel;
            mFirstFrameNumOfCurLabel = it.mStartFrameNum;
            mLastFrameNumOfCurLabel = it.mEndFrameNum;
            mIsHandlerValid = true;
            mIsLoop = isLoop;


            mAnimState = kAnimStatePlaying;

            MYPRINT("-Node:PlaySection Success\n");
            return true;
        }
    }

    mIsHandlerValid = false;

    print_line("Could not find the specified label in animation.");
    return false;
}

void SuperAnimNode::superAnimDraw()
{
    if (mAnimState == kAnimStateInvalid ||
            mAnimState == kAnimStateInitialized)
    {
        return;
    }

    if (!IsValid()) {
        return;
    }

//    MYPRINT("+SuperAnimNode::superAnimDraw()\n");


    static SuperAnim::SuperAnimObjDrawInfo sAnimObjDrawnInfo;
    //float aPixelToPointScale = 1.0f / CC_CONTENT_SCALE_FACTOR();
    float anAnimContentHeightInPixel = mHeight;
    SuperAnim::BeginIterateAnimObjDrawInfo();

    while (SuperAnim::IterateAnimObjDrawInfo(this, sAnimObjDrawnInfo)) {
        if (sAnimObjDrawnInfo.mSpriteId == InvalidSuperAnimSpriteId) {
//            assert(false, "Missing a sprite.");
            print_line("missing a sprite");
            continue;
        }

        // check whether this sprite has been replaced
        SuperAnim::SuperAnimSpriteId aCurSpriteId = sAnimObjDrawnInfo.mSpriteId;

        //TODO: Handling the sprite replace situation.
//        SuperAnim::SuperSpriteIdToSuperSpriteIdMap::Element *anIter = mReplacedSpriteMap.find(aCurSpriteId);
//        if (anIter != mReplacedSpriteMap.end()) {
//            aCurSpriteId = anIter->second;
//        }

        //SuperAnimSprite *aSprite = SuperAnimSpriteMgr::GetInstance()->GetSpriteById(aCurSpriteId);
        SuperAnim::SuperAnimSprite *aSprite = (SuperAnim::SuperAnimSprite*)aCurSpriteId;
        if (aSprite == NULL){
//            assert(false, "Missing a sprite.");
            print_line("Missing a sprite.");
            continue;
        }

        ccV3F_C4B_T2F_Quad aOriginQuad = aSprite->mQuad;
//        MYPRINT("Pointer: %p\n", aSprite);
//        MYPRINT("Point: (%f, %f, %f)\n", aOriginQuad.bl.vertices.x,  aOriginQuad.bl.vertices.y, aOriginQuad.bl.vertices.z);
//        MYPRINT("Point: (%f, %f, %f)\n", aOriginQuad.br.vertices.x,  aOriginQuad.br.vertices.y, aOriginQuad.br.vertices.z);
//        MYPRINT("Point: (%f, %f, %f)\n", aOriginQuad.tl.vertices.x,  aOriginQuad.tl.vertices.y, aOriginQuad.tl.vertices.z);
//        MYPRINT("Point: (%f, %f, %f)\n", aOriginQuad.tr.vertices.x,  aOriginQuad.tr.vertices.y, aOriginQuad.tr.vertices.z);

        //TODO: enable sprite sheeets.
//        // safe check!!
//        if (mUseSpriteSheet) {
//            CCAssert(mSpriteSheet == aSprite->mTexture, "must in the same texture!!");
//        }

        // cocos2d the origin is located at left bottom, but is in left top in flash
//        sAnimObjDrawnInfo.mTransform.mMatrix.m12 = anAnimContentHeightInPixel - sAnimObjDrawnInfo.mTransform.mMatrix.m12;

        //FIX: rotation fix on the matrix so the animations look fine.
        sAnimObjDrawnInfo.mTransform.mMatrix.m01 *= -1;
        sAnimObjDrawnInfo.mTransform.mMatrix.m10 *= -1;

        aSprite->mQuad = sAnimObjDrawnInfo.mTransform.mMatrix * aSprite->mQuad;
        ccColor4B aColor = ccc4(sAnimObjDrawnInfo.mColor.mRed, sAnimObjDrawnInfo.mColor.mGreen, sAnimObjDrawnInfo.mColor.mBlue, sAnimObjDrawnInfo.mColor.mAlpha);
        aSprite->mQuad.bl.colors = aColor;
        aSprite->mQuad.br.colors = aColor;
        aSprite->mQuad.tl.colors = aColor;
        aSprite->mQuad.tr.colors = aColor;

        if (mIsFlipX) {
            float aWidthinPixel = mWidth;
            aSprite->mQuad.bl.vertices.x = aWidthinPixel - aSprite->mQuad.bl.vertices.x;
            aSprite->mQuad.br.vertices.x = aWidthinPixel - aSprite->mQuad.br.vertices.x;
            aSprite->mQuad.tl.vertices.x = aWidthinPixel - aSprite->mQuad.tl.vertices.x;
            aSprite->mQuad.tr.vertices.x = aWidthinPixel - aSprite->mQuad.tr.vertices.x;
        }

        if (mIsFlipY) {
            float aHeightinPixel = mHeight;
            aSprite->mQuad.bl.vertices.y = aHeightinPixel - aSprite->mQuad.bl.vertices.y;
            aSprite->mQuad.br.vertices.y = aHeightinPixel - aSprite->mQuad.br.vertices.y;
            aSprite->mQuad.tl.vertices.y = aHeightinPixel - aSprite->mQuad.tl.vertices.y;
            aSprite->mQuad.tr.vertices.y = aHeightinPixel - aSprite->mQuad.tr.vertices.y;
        }

        // draw
//        if (!mUseSpriteSheet)
        {
            p_points[0].x = aSprite->mQuad.tl.vertices.x;
            p_points[0].y = aSprite->mQuad.tl.vertices.y;
            p_uvs[0].x = aSprite->mQuad.tl.texCoords.u;
            p_uvs[0].y = aSprite->mQuad.tl.texCoords.v;

            p_points[1].x = aSprite->mQuad.bl.vertices.x;
            p_points[1].y = aSprite->mQuad.bl.vertices.y;
            p_uvs[1].x = aSprite->mQuad.bl.texCoords.u;
            p_uvs[1].y = aSprite->mQuad.bl.texCoords.v;

            p_points[2].x = aSprite->mQuad.br.vertices.x;
            p_points[2].y = aSprite->mQuad.br.vertices.y;
            p_uvs[2].x = aSprite->mQuad.br.texCoords.u;
            p_uvs[2].y = aSprite->mQuad.br.texCoords.v;

            p_points[3].x = aSprite->mQuad.tr.vertices.x;
            p_points[3].y = aSprite->mQuad.tr.vertices.y;
            p_uvs[3].x = aSprite->mQuad.tr.texCoords.u;
            p_uvs[3].y = aSprite->mQuad.tr.texCoords.v;

            Color c(aColor.r/255.0, aColor.g/255.0, aColor.b/255.0, aColor.a/255.0);

            draw_colored_polygon(p_points, c, p_uvs, aSprite->mTexRef);
//            draw_colored_polygon(p_points, Color(0, 0.5, 0, 0.2));
        }

        aSprite->mQuad = aOriginQuad;
    }
//    MYPRINT("-SuperAnimNode::superAnimDraw()\n");
}

void SuperAnimNode::_notification(int p_what){
    switch(p_what) {
        case NOTIFICATION_PROCESS:{
            Node::_notification(p_what);
            float dt = get_process_delta_time();

//            myColor.g += dt;
//            myColor.a -= dt;
            
//            if(myColor.g > 1){
//                myColor.g = 0;
//            }
            
//            if(myColor.a < 0.2){
//                myColor.a = 1;
//            }

            superAnimUpdate(dt);

            update();
            break;
        }
            
        //node gets a notification whenever there is an update in display required.
        case NOTIFICATION_DRAW: {
//            RID ci = get_canvas_item();
//            Rect2 rect = Rect2(0, 0, 100, 100);
//            draw_rect(rect, myColor);
            if(showAxis){
                draw_line(Vector2(0, 0), Vector2(0, 100), Color(0, 0, 1), 2);
                draw_line(Vector2(0, 0), Vector2(100, 0), Color(1, 0, 0), 2);
            }
            if(showStage){
                Rect2 rect = Rect2(0, 0, mWidth, mHeight);
                draw_rect(rect, Color(1, 1, 1, 0.2));
            }
            superAnimDraw();
            break;
        }
    }
}
