# Quickstart:本地人脸识别认证端到端验证

**Date**: 2026-08-21

## 0. 前置条件

- kiran-authentication-service 已安装并包含 face 驱动插件与 FaceDevice(本仓库配套交付)
- 摄像头可用;模型已安装至 `$KAS_INSTALL_DATADIR/models/face/`
  (模型由 kiran-authentication-devices-sdk 仓库提供,不随服务仓库分发;
  开发调试可用 `KAS_FACE_MODEL_DIR` 指向本地模型目录)
- 系统服务运行:`kiran-authentication-devices`、`kiran-authentication-daemon`
- 本仓库认证插件已构建部署(`kiran-cpanel-authentication`)

## 1. 面板端到端验证(主路径)

| # | 步骤 | 期望 |
|---|---|---|
| 1 | 控制面板 → 身份认证 → 人脸 | 特征列表正常显示 |
| 2 | 点击"录入" → 密码验证 | 切入录入页:先黑底,首帧后出摄像头画面,无上一页残留 |
| 3 | 面对摄像头,点击"拍照/录入" | 进度反馈;成功弹窗,特征出现在列表 |
| 4 | 无脸/多脸照片点拍照 | 无脸提示"未检测到人脸";多脸取面积最大的一张录入 |
| 5 | 同一张脸再次录入 | 提示"该特征已录入" |
| 6 | 已有 10 个特征时再次录入 | 拒绝并提示"特征数量已达上限" |
| 7 | 重命名/删除特征 → 重启服务 | 状态保持;删除后不再参与认证 |
| 8 | 录入页停留 2 分钟 | 自动关闭预览并返回管理页 |
| 9 | 录入中取消/关闭页面 | 流程终止、摄像头关闭、无残留特征 |
| 10 | 缩放面板窗口 | 预览随窗口缩放(最小 272×272),画面等比不变形 |
| 11 | 锁屏 → 人脸认证,本人 | 认证通过 |
| 12 | 锁屏 → 人脸认证,他人 | 提示"人脸不匹配",可切换密码 |
| 13 | 拔掉摄像头后发起人脸认证 | 提示"无法打开摄像头"(非笼统"特征不匹配") |
| 14 | 认证中切换密码 | 识别停止、切换流畅 |
| 15 | 驱动管理关闭人脸驱动 | 开关保持关闭;锁屏人脸消失;重开恢复;重启服务状态保持 |
| 16 | kad.ini [Password] Enable=false 并重启 daemon | 锁屏只剩人脸;恢复 true 后密码回来 |
| 17 | 断网后重复 1-16 | 全部正常(全离线) |

## 2. 服务侧 D-Bus 直接验证(跳过面板)

```bash
# 录入:注意 dbus-send 单参数上限 128KB,压缩图片后再编码
IMG_B64=$(base64 -w0 face.jpg)
JSON="{\"faceImage\":\"$IMG_B64\"}"
dbus-send --system --print-reply --dest=com.kylinsec.Kiran.Authentication \
  /com/kylinsec/Kiran/Authentication \
  com.kylinsec.Kiran.Authentication.FindUserByID uint64:0
dbus-send --system --print-reply --dest=com.kylinsec.Kiran.Authentication \
  /com/kylinsec/Kiran/Authentication/User/0 \
  com.kylinsec.Kiran.Authentication.User.EnrollStart \
  int32:4 string:"face1" string:"$JSON"
# 期望:dbus-monitor 观察到 EnrollStatus 信号,最终 (iid, true, 100, ...);
# GetIdentifications(face) 返回新特征;重启 daemon 后仍在
```

识别等长流程需用保持 D-Bus 连接的脚本(dbus-send 调用完即退出会触发断连自动停止):

```bash
python3 - <<'EOF'
import dbus, dbus.mainloop.glib, json
from gi.repository import GLib
dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
bus = dbus.SystemBus()
# 设备路径用 GetDevicesByType(int32:1) 查询
obj = bus.get_object('com.kylinsec.Kiran.AuthDevice', '/com/kylinsec/Kiran/AuthDevice/Device_XXX')
iface = dbus.Interface(obj, 'com.kylinsec.Kiran.AuthDevice.Device')
iface.connect_to_signal('IdentifyStatus', lambda fid, result, msg: print('IdentifyStatus:', fid, result, msg))
iface.IdentifyStart(json.dumps({'feature_ids': ['<featureID>']}))
loop = GLib.MainLoop(); GLib.timeout_add_seconds(15, loop.quit); loop.run()
EOF
```

## 3. 成功标准对照(spec SC)

| SC | 验证方式 |
|---|---|
| SC-001 不阻塞 PAM 链路 | 步骤 14 |
| SC-002 本人通过率 ≥95% | 步骤 11 重复 20 次 |
| SC-003 他人误通过 ≤0.1% | 多人交叉测试(≥1000 次,可用批量图集) |
| SC-004 首次录入成功率 ≥90% | 步骤 2-3 重复 20 次 |
| SC-005 增删改查即时生效 | 步骤 7 |
| SC-006 离线可用 | 步骤 17 |

## 4. 已知限制

- 无活体检测:照片/屏幕可通过
- 仅默认 V4L2 摄像头
- 无人走监测(后续迭代)
- 大窗口快速运动时预览可能撕裂(Qt5 V4L2 后端限制,暂接受)
- 重复录入判定基于与库中特征的余弦相似度(阈值同识别阈值);识别摄像头由
  kad.ini [Face] Camera 配置(索引或设备路径,默认 0)
