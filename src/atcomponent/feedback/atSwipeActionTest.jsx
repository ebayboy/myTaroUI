
/* isOponed 实际上是false的， flase的情况下 取消和确认按钮不弹出来 */

import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'
import { AtSwipeAction, AtButton } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

//import './AtSwipeActionTest.scss';

const INIT_STATE = {
    isOpened: false,
    disabled: false,
    autoClose: false,
    options: [{ text: '取消', style: { backgroundColor: '#6190E8' } }, { text: '确认', style: { backgroundColor: '#FF4949' } }]
}

export default class AtSwipeActionTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { ...INIT_STATE }
        console.log(this.state)
    }

    handlerClickBtn(disabled, autoClose, options) {

        console.log(this.state.isOpened)

        if (this.state.isOpened === true) {
            return this.setState({ ...INIT_STATE })
        }

        const state = Object.assign(
            { ...INIT_STATE },
            { disabled: disabled, autoClose: autoClose, options: options }
        )

        this.setState(state)
        console.log(state)
    }

    handlerClickSA() {
        console.log("isOpend:", this.state.isOpened)
        console.log(this.handlerClickSA.name)
    }

    handleronOpenedSA() {
        console.log("isOpend:", this.state.isOpened)
        console.log(this.handleronOpenedSA.name)
    }

    handleronClosedSA() {
        console.log("isOpend:", this.state.isOpened)
        console.log(this.handleronClosedSA.name)
    }

    render() {
        const { isOpened, disabled, autoClose, options } = this.state

        console.log("isOpened:", isOpened)

        return (
            <View>
                <View>
                    <AtButton type='primary' size='small'
                        onClick={this.handlerClickBtn.bind(this, false, false,
                            [{ text: '取消', style: { backgroundColor: '#6190E8' } }, { text: '确认', style: { backgroundColor: '#FF4949' } }]
                        )} >一般使用场景</AtButton>
                </View>

                <AtSwipeAction
                    isOpened={isOpened}
                    disabled={disabled}
                    autoClose={autoClose}
                    options={options}
                    onClick={this.handlerClickSA.bind(this)}
                    onClosed={this.handleronClosedSA.bind(this)}
                    onOpened={this.handleronOpenedSA.bind(this)}
                >
                    <View>SwiperActionTest</View>
                </AtSwipeAction>
            </View >
        )
    }
}