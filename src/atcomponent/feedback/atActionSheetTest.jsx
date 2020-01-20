
import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtActionSheet, AtActionSheetItem } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

export default class AtActionSheetTest extends Component {
    constructor() {
        super(...arguments)

        this.state = {
            isOpened: true
        }
    }

    handleCancel = () => {
        Taro.showToast({
            icon: 'none',
            title: '点击了取消按钮'
        })
    }

    handleClose = () => {
        this.setState({ isOpened: false })
    }

    showToast(name) {
        Taro.showToast({
            icon: 'none',
            title: name
        })
    }

    render() {
        const isOpened = this.state

        return (
            <View>
                <AtActionSheet isOpened= {isOpened}
                    cancelText='取消'
                    title='头部标题可以用通过转义字符换行'
                    onCancel={this.handleCancel}
                    onClose={this.handleClose}
                >
                    <AtActionSheetItem
                        onClick={this.showToast.bind(this, '点击了按钮一')}
                    >
                        按钮一
                    </AtActionSheetItem>
                    <AtActionSheetItem
                        onClick={this.showToast.bind(this, '点击了按钮二')}
                    >
                        按钮二
                    </AtActionSheetItem>
                </AtActionSheet>
            </View >
        )
    }
}