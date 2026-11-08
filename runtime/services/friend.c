// friend:u / friend:v / friend:m / friend:s / friend:a (nn::friends): the Switch friend list, answered as a console
// that is offline with no friends. Pokémon Sword / Shield open it at start-up (Let's Go never did): the logging stub
// answered CreateFriendService "ok" without a session object, and the game's SDK code then read through a null pointer
// right after (exception c0000005 accessing 0 in the sdk's nn::friends code).
//
// Commands from switchbrew "Friend services". Counts and flags come back 0 (no friends, no requests, no blocked users,
// nothing cached), lists empty; anything else is logged and answered with zeroed outputs, so a command the game needs
// more from shows up in the log.
#include <string.h>

#include "../hle.h"
#include "../ipc.h"

// An event already signalled: the friend service's completion / notification events. Async friend operations here
// finish at once (there is nothing to fetch), so a wait on them returns immediately instead of hanging the game.
static uint32_t signalled_event(void) {
    uint32_t w, r;
    KEvent *e = kernel_create_event_pair(&w, &r);
    if (e) e->signaled = true;  // (before the handle is given out: no thread can be waiting on it yet)
    return r;
}

// Result for INotificationService::Pop with nothing queued (module 124, description 15, as other HLE
// implementations of this service return)
#define RESULT_NO_NOTIFICATION (124u | (15u << 9))

// ------------------------------------------------------------------ IFriendService
static void friend_service(Service *s, IpcReq *req, IpcResp *resp) {
    (void)s;
    switch (req->cmd) {
    case 0:  // GetCompletionEvent
        resp->copy_handles[resp->ncopy++] = signalled_event();
        break;
    case 1:  // Cancel
        break;
    case 10100:  // GetFriendListIds -> s32 count (the ids buffer left empty)
    case 10101:  // GetFriendList -> s32 count
    case 10400:  // GetBlockedUserListIds -> s32 count
    case 20100:  // GetFriendCount -> s32
    case 20101:  // GetNewlyFriendCount -> s32
    case 20400:  // GetBlockedUserList -> s32 count
    case 20700:  // GetPlayHistoryList -> s32 count
        OUT(int32_t, 0) = 0;
        resp_size(resp, 4);
        break;
    case 10120:  // IsFriendListCacheAvailable -> bool
        OUT(uint8_t, 0) = 0;
        resp_size(resp, 4);
        break;
    case 20200:  // GetReceivedFriendRequestCount -> s32 unread, s32 read
        OUT(int32_t, 0) = 0, OUT(int32_t, 4) = 0;
        resp_size(resp, 8);
        break;
    case 10600:  // DeclareOpenOnlinePlaySession
    case 10601:  // DeclareCloseOnlinePlaySession
    case 10610:  // UpdateUserPresence
    case 10702:  // AddPlayHistory
    case 20103:  // SyncFriendList
    case 20104:  // RequestSyncFriendList
    case 20401:  // SyncBlockedUserList
    case 20801:  // SyncUserSetting
        break;  // (nothing to do offline)
    default:
        LOG("[friend] IFriendService cmd %u (ok, zeroed)\n", req->cmd);
        resp_size(resp, 0x40);
    }
}

// ------------------------------------------------------------------ INotificationService
static void notification_service(Service *s, IpcReq *req, IpcResp *resp) {
    (void)s;
    switch (req->cmd) {
    case 0:  // GetEvent (never a notification: signalled once so a first wait returns, then Pop says there's none)
        resp->copy_handles[resp->ncopy++] = signalled_event();
        break;
    case 1:  // Clear
        break;
    case 2:  // Pop
        resp->result = RESULT_NO_NOTIFICATION;
        break;
    default:
        LOG("[friend] INotificationService cmd %u (ok)\n", req->cmd);
    }
}

// ------------------------------------------------------------------ IDaemonSuspendSessionService
static void suspend_session_service(Service *s, IpcReq *req, IpcResp *resp) {
    (void)s, (void)resp;
    LOG("[friend] IDaemonSuspendSessionService cmd %u (ok)\n", req->cmd);
}

// ------------------------------------------------------------------ IServiceCreator (every friend:* port)
static void service_creator(Service *s, IpcReq *req, IpcResp *resp) {
    switch (req->cmd) {
    case 0:  // CreateFriendService
        ipc_return_object(resp, service_new("IFriendService", friend_service, NULL));
        break;
    case 1:  // CreateNotificationService(uid)
        ipc_return_object(resp, service_new("INotificationService", notification_service, NULL));
        break;
    case 2:  // CreateDaemonSuspendSessionService
        ipc_return_object(resp, service_new("IDaemonSuspendSessionService", suspend_session_service, NULL));
        break;
    default:
        LOG("[friend] %s cmd %u (ok)\n", s->name, req->cmd);
    }
}

SERVICE("friend:u", friend_u) { service_creator(s, req, resp); }
SERVICE("friend:v", friend_v) { service_creator(s, req, resp); }
SERVICE("friend:m", friend_m) { service_creator(s, req, resp); }
SERVICE("friend:s", friend_s) { service_creator(s, req, resp); }
SERVICE("friend:a", friend_a) { service_creator(s, req, resp); }
