import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtTabBar } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

export default class AtTabBarTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { current: 0 }
    }

    handleClick(value) {
        this.setState({ current: value })
    }

    render() {
        return (
            <View>
                <View>tabbar一般用法</View>
                <AtTabBar current={this.state.current}
                    onClick={this.handleClick.bind(this)}
                    tabList={[
                        { title: '待办事项', text: 8 },
                        { title: '拍照' },
                        { title: '通讯录', dot: true }
                    ]}
                />
                <View>tabbar-带图标用法</View>
                <AtTabBar
                    tabList={[
                        { title: '待办事项', iconType: 'bullet-list', text: 'new' },
                        { title: '拍照', iconType: 'camera' },
                        { title: '文件夹', iconType: 'folder', text: '100', max: '99' }
                    ]}
                    onClick={this.handleClick.bind(this)}
                    current={this.state.current}
                />
                <View>tabbar-自定义【重要】</View>
                <AtTabBar
                    backgroundColor='#ececec'
                    color='#ea6bb8'
                    tabList={[
                        { title: '待办事项', iconType: 'bullet-list', text: 'new' },
                        { title: '拍照', iconType: 'camera' },
                        { title: '文件夹', iconType: 'folder', text: '100', max: '99' }
                    ]}
                    onClick={this.handleClick.bind(this)}
                    current={this.state.current}
                />
                <View>tabbar-使用图片作为图标</View>
                <AtTabBar
                    tabList={[
                        { title: '领取中心', image: 'https://img12.360buyimg.com/jdphoto/s72x72_jfs/t6160/14/2008729947/2754/7d512a86/595c3aeeNa89ddf71.png', selectedImage: 'https://img14.360buyimg.com/jdphoto/s72x72_jfs/t17251/336/1311038817/3177/72595a07/5ac44618Na1db7b09.png', text: 'new' },
                        { title: '找折扣', image: 'https://img20.360buyimg.com/jdphoto/s72x72_jfs/t15151/308/1012305375/2300/536ee6ef/5a411466N040a074b.png' },
                        { title: '领会员', image: 'https://img10.360buyimg.com/jdphoto/s72x72_jfs/t5872/209/5240187906/2872/8fa98cd/595c3b2aN4155b931.png', text: '100', max: '99' }
                    ]}
                    onClick={this.handleClick.bind(this)}
                    current={this.state.current}
                />
            </View>
        )
    }
}