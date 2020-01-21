import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtModal, AtButton } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

//import './AtModalTest.scss';

export default class AtModalTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { isOpened: false }
    }

    handlerCancle() {
        console.log("cancle isOpened:", this.state.isOpened)
        this.setState({ isOpened: false })
    }

    handleConfirm() {
        console.log("confirm isOpened:", this.state.isOpened)
        this.setState({ isOpened: false })
    }

    handleClose() {
        console.log("close isOpened:", this.state.isOpened)
        this.setState({ isOpened: false })
    }

    handlerBtnClick() {
        console.log("close isOpened:", this.state.isOpened)
        this.setState({ isOpened: true })
    }

    render() {
        const isOpened = this.state.isOpened

        return (
            <View>
                <AtButton type='primary' size='small' onClick={this.handlerBtnClick.bind(this)}>Open Modal</AtButton>
                <AtModal isOpened={isOpened}
                    title='标题'
                    cancelText='取消'
                    confirmText='确定'
                    content='京东云VPCWAF\n\r京东云VPCWAF\n\n'
                    onCancel={this.handlerCancle.bind(this)}
                    onConfirm={this.handleConfirm.bind(this)}
                    onClose={this.handleClose.bind(this)}
                >
                </AtModal>
            </View>
        )
    }
}