import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtFloatLayout, AtButton } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtFloatLayoutTest extends Component {
    constructor() {
        super(...arguments)

        this.state = { isOpened: false }
    }

    handleClose() {
        this.setState({ isOpened: false })
    }

    handleClick() {
        this.setState({ isOpened: true })
    }

    render() {
        return (
            <View>
                <AtButton onClick={this.handleClick.bind(this)}>打开内容区</AtButton>
                <AtFloatLayout isOpened={this.state.isOpened} title='基本用法' onClose={this.handleClose.bind(this)}>
                    这是内容区 随你怎么写这是内容区 随你怎么写这是内容区 随你怎么写这是内容区
      随你怎么写这是内容区 随你怎么写这是内容区 随你怎么写
                </AtFloatLayout>
            </View>
        )
    }
}