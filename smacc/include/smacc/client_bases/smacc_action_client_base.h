#pragma once

#include <smacc/client_bases/smacc_action_client.h>
#include <boost/signals2.hpp>
#include <boost/optional/optional_io.hpp>

namespace smacc
{
using namespace boost::signals2;

template <typename ActionType>
class SmaccActionClientBase : public ISmaccActionClient
{
public:
    // Inside this macro you can find the typedefs for Goal and other types
    ACTION_DEFINITION(ActionType);
    typedef actionlib::SimpleActionClient<ActionType> ActionClient;
    typedef actionlib::SimpleActionClient<ActionType> GoalHandle;

    typedef typename ActionClient::SimpleDoneCallback SimpleDoneCallback;
    typedef typename ActionClient::SimpleActiveCallback SimpleActiveCallback;
    typedef typename ActionClient::SimpleFeedbackCallback SimpleFeedbackCallback;

    SmaccActionClientBase()
        : ISmaccActionClient()
    {
    }

    static std::string getEventLabel()
    {
        auto type = TypeInfo::getTypeInfoFromType<ActionType>();
        return type->getNonTemplatetypename();
    }

    virtual ~SmaccActionClientBase()
    {
    }

    /// rosnamespace path
    boost::optional<std::string> name_;

    virtual void initialize() override
    {
        if (!name_)
        {
            name_ = demangleSymbol(typeid(*this).name());
            std::vector<std::string> strs;
            boost::split(strs, *name_, boost::is_any_of("::"));
            std::string classname = strs.back();
            name_ = classname;
        }

        client_ = std::make_shared<ActionClient>(*name_);
    }

    signal<void(const ResultConstPtr &)> onSucceeded_;
    signal<void(const ResultConstPtr &)> onAborted_;
    signal<void(const ResultConstPtr &)> onPreempted_;
    signal<void(const ResultConstPtr &)> onCancelled_;
    signal<void(const ResultConstPtr &)> onRejected_;

    // event creation/posting factory functions
    std::function<void(ResultConstPtr)> postSuccessEvent;
    std::function<void(ResultConstPtr)> postAbortedEvent;
    std::function<void(ResultConstPtr)> postPreemptedEvent;
    std::function<void(ResultConstPtr)> postRejectedEvent;

    std::function<void(FeedbackConstPtr)> postFeedbackEvent;

    SimpleDoneCallback done_cb;
    SimpleActiveCallback active_cb;
    SimpleFeedbackCallback feedback_cb;

    template <typename EvType>
    void postResultEvent(ResultConstPtr result)
    {
        auto *ev = new EvType();
        ev->client = this;
        ev->resultMessage = *result;
        ROS_INFO("Posting EVENT %s", demangleSymbol(typeid(ev).name()).c_str());
        this->postEvent(ev);
    }

    template <typename TDerived, typename TObjectTag>
    void assignToOrthogonal()
    {
        // we create here all the event factory functions capturing the TObjectTag
        postSuccessEvent = [=](auto msg) { this->postResultEvent<EvActionSucceeded<TDerived, TObjectTag>>(msg); };
        postAbortedEvent = [=](auto msg) { this->postResultEvent<EvActionAborted<TDerived, TObjectTag>>(msg); };
        postPreemptedEvent = [=](auto msg) { this->postResultEvent<EvActionPreempted<TDerived, TObjectTag>>(msg); };
        postRejectedEvent = [=](auto msg) { this->postResultEvent<EvActionRejected<TDerived, TObjectTag>>(msg); };
        postFeedbackEvent = [=](auto msg) {
            auto actionFeedbackEvent = new EvActionFeedback<Feedback, TObjectTag>();
            actionFeedbackEvent->client = this;
            actionFeedbackEvent->feedbackMessage = *msg;
            this->postEvent(actionFeedbackEvent);
            ROS_WARN("FEEDBACK EVENT");
        };

        done_cb = boost::bind(&SmaccActionClientBase<ActionType>::onResult, this, _1, _2);
        //active_cb;
        feedback_cb = boost::bind(&SmaccActionClientBase<ActionType>::onFeedback, this, _1);
    }

    template <typename T>
    connection onSucceeded(void (T::*callback)(ResultConstPtr &), T *object)
    {
        return this->onSucceeded_.connect([&](auto msg) { (object->*callback)(msg); });
    }

    template <typename T>
    connection onSucceeded(std::function<void(ResultConstPtr &)> callback)
    {
        return this->onSucceeded_.connect(callback);
    }

    template <typename T>
    connection onAborted(void (T::*callback)(ResultConstPtr &), T *object)
    {
        return this->onAborted_.connect([&](auto msg) { (object->*callback)(msg); });
    }

    template <typename T>
    connection onAborted(std::function<void(ResultConstPtr &)> callback)
    {
        return this->onAborted_.connect(callback);
    }

    template <typename T>
    connection onPreempted(void (T::*callback)(ResultConstPtr &), T *object)
    {
        return this->onPreempted_.connect([&](auto msg) { (object->*callback)(msg); });
    }

    template <typename T>
    connection onPreempted(std::function<void(ResultConstPtr &)> callback)
    {
        return this->onPreempted_.connect(callback);
    }

    template <typename T>
    connection onRejected(void (T::*callback)(ResultConstPtr &), T *object)
    {
        return this->onRejected_.connect([&](auto msg) { (object->*callback)(msg); });
    }

    template <typename T>
    connection onRejected(std::function<void(ResultConstPtr &)> callback)
    {
        return this->onRejected_.connect(callback);
    }

    virtual void cancelGoal() override
    {
        ROS_INFO("Cancelling goal of %s", this->getName().c_str());
        client_->cancelGoal();
    }

    virtual SimpleClientGoalState getState() override
    {
        return client_->getState();
    }

    void sendGoal(Goal &goal)
    {
        ROS_INFO_STREAM("Sending goal to actionserver located in " << *(this->name_) << "\"");

        if (!client_->isServerConnected())
        {
            ROS_INFO("%s [at %s]: not connected with actionserver, waiting ...", getName().c_str(), getNamespace().c_str());
            client_->waitForServer();
        }

        ROS_INFO_STREAM(getName() << ": Goal Value: " << std::endl << goal);

        client_->sendGoal(goal, done_cb, active_cb, feedback_cb);
    }

protected:
    std::shared_ptr<ActionClient> client_;

    void onFeedback(const FeedbackConstPtr &feedback_msg)
    {
        postFeedbackEvent(feedback_msg);
    }

    void onResult(const SimpleClientGoalState &state, const ResultConstPtr &result_msg)
    {
        // auto *actionResultEvent = new EvActionResult<TDerived>();
        // actionResultEvent->client = this;
        // actionResultEvent->resultMessage = *result_msg;

        const auto &resultType = this->getState();
        ROS_INFO("[%s] request result: %s", this->getName().c_str(), resultType.toString().c_str());

        if (resultType == actionlib::SimpleClientGoalState::SUCCEEDED)
        {
            ROS_INFO("[%s] request result: Success", this->getName().c_str());
            onSucceeded_(result_msg);
            postSuccessEvent(result_msg);
        }
        else if (resultType == actionlib::SimpleClientGoalState::ABORTED)
        {
            ROS_INFO("[%s] request result: Aborted", this->getName().c_str());
            onAborted_(result_msg);
            postAbortedEvent(result_msg);
        }
        else if (resultType == actionlib::SimpleClientGoalState::REJECTED)
        {
            ROS_INFO("[%s] request result: Rejected", this->getName().c_str());
            onRejected_(result_msg);
            postRejectedEvent(result_msg);
        }
        else if (resultType == actionlib::SimpleClientGoalState::PREEMPTED)
        {
            ROS_INFO("[%s] request result: Preempted", this->getName().c_str());
            onPreempted_(result_msg);
            postPreemptedEvent(result_msg);
        }
        else
        {
            ROS_INFO("[%s] request result: NOT HANDLED TYPE: %s", this->getName().c_str(), resultType.toString().c_str());
        }
    }
};
} // namespace smacc