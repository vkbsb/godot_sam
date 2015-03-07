/* sumator.h */
#ifndef SUPER_ANIM_NODE_H
#define SUPER_ANIM_NODE_H

#include "scene/2d/node_2d.h"
#include "SuperAnimResource.h"
#include "SuperAnimSprite.h"

class SuperAnimNode : public Node2D, public SuperAnim::SuperAnimHandler {
    OBJ_TYPE(SuperAnimNode, Node2D);

    int count;
    Color myColor;
    int mId;

    bool mIsFlipX;
    bool mIsFlipY;
    float mSpeedFactor;
    bool mIsLoop;
    AnimState mAnimState;

    //properties to show.
    bool showStage;
    bool showAxis;

    Ref<SuperAnimData> mSamRes;
    StringArray mSections;

    Vector<Point2> p_points;
    Vector<Point2> p_uvs;


    void superAnimDraw();
    void superAnimUpdate(float dt);

    protected:
    static void _bind_methods();

    public:

    bool PlaySection(const String &labelName, bool isLoop);
    bool HasSection(const String &labelName);
    void setSpeedFactor(float newSpeedFactor);

    void Pause();
    void Resume();
    bool IsPause();
    bool IsPlaying();
    int GetCurFrame();
    int GetId();

    //set the section that is played now.
    String GetCurSectionName();
    void SetCurSectionName(String section);

    float get_width();
    float get_height();

    void set_showstage(bool p_ShowStage);
    bool is_showstage() const;

    void set_showaxis(bool p_ShowAxis);
    bool is_showaxis() const;

    void set_sam(const Ref<SuperAnimData>);
    Ref<SuperAnimData> get_sam() const;

    //load the resource string and get the corresponding handle.
    void load_anim(String resPath);

    //function for handling callbacks for engine events.
    void _notification(int p_what);
        
    SuperAnimNode();
};

#endif
