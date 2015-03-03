/* sumator.cpp */

#include "SuperAnimNode.h"
#include "SuperAnimSprite.h"

#include "core/io/file_access_buffered.h"
#include "core/io/resource_loader.h"

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

String SuperAnimNode::GetCurSectionName(){
    return mCurLabel;
}


void SuperAnimNode::add(int value) {

    count+=value;
}

void SuperAnimNode::reset() {

    count=0;
}

int SuperAnimNode::get_total() const {

    return count;
}

float SuperAnimNode::get_width(){
    return this->mWidth;
}

float SuperAnimNode::get_height(){
    return this->mHeight;
}

void SuperAnimNode::_bind_methods() {

    ObjectTypeDB::bind_method("add",&SuperAnimNode::add);
    ObjectTypeDB::bind_method("reset",&SuperAnimNode::reset);
    ObjectTypeDB::bind_method("get_total",&SuperAnimNode::get_total);
    ObjectTypeDB::bind_method("get_height", &SuperAnimNode::get_height);
    ObjectTypeDB::bind_method("get_width", &SuperAnimNode::get_width);
    ObjectTypeDB::bind_method("load_anim", &SuperAnimNode::load_anim);


//    ADD_PROPERTY( PropertyInfo( Variant::Int, "width"), _SCS("set_offset"),_SCS("get_offset"));

}

SuperAnimNode::SuperAnimNode() {
    print_line("+SuperAnimNode::SuperAnimNode()");
    count=0;
    myColor = Color(1, 0, 0, 1);
    
    print_line("Constructor");
    
    set_process(true);

    print_line("-SuperAnimNode::SuperAnimNode()");
}


void SuperAnimNode::load_anim(String resPath)
{
    Ref<SuperAnimData> samRes = ResourceLoader::load(resPath);

    //we merge the details of SuerAnimDef into Node.
    if( !samRes.is_null() ){
        SuperAnimData *aMainDef = samRes.ptr();
        this->mMainDefKey = aMainDef->get_path();
        this->mAnimRate = aMainDef->mAnimRate;
        this->mWidth = aMainDef->mWidth;
        this->mHeight = aMainDef->mHeight;
        this->mCurFrameNum = aMainDef->mStartFrameNum;
        this->mIsHandlerValid = true;
    } else {
        this->mIsHandlerValid = false;
    }
}

void SuperAnimNode::_notification(int p_what){
    switch(p_what) {
        case NOTIFICATION_PROCESS:{
            Node::_notification(p_what);
            float dt = get_process_delta_time();
            myColor.g += dt;
            myColor.a -= dt;
            
            if(myColor.g > 1){
                myColor.g = 0;
            }
            
            if(myColor.a < 0.2){
                myColor.a = 1;
            }
            update();
            break;
        }
            
        //node gets a notification whenever there is an update in display required.
        case NOTIFICATION_DRAW: {
            RID ci = get_canvas_item();
            Rect2 rect = Rect2(0, 0, 100, 100);
            draw_rect(rect, myColor);
            break;
        }
    }
}
