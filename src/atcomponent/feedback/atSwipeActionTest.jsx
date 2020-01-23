
/* isOponed 实际上是false的， flase的情况下 取消和确认按钮不弹出来 */

import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'
import { AtSwipeAction, AtButton, AtList, AtListItem } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

//import './AtSwipeActionTest.scss';

const INIT_STATE = {
    showText: '一般使用场景',
    isOpened: false,
    disabled: false,
    autoClose: false,
    options: [{ text: '取消', style: { backgroundColor: '#6190E8' } }, { text: '确认', style: { backgroundColor: '#FF4949' } }],
    list: [{ title: 'item_map1', isOpened: false }, { title: 'item_map2', isOpened: false }, { title: 'item_map3', isOpened: false }]
}
export default class AtSwipeActionTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { ...INIT_STATE }
        console.log(this.state)
    }

    handlerClickBtn(disabled, autoClose, options, showText) {

        console.log(this.state.isOpened)

        if (this.state.isOpened === true) {
            return this.setState({ ...INIT_STATE })
        }

        const state = Object.assign(
            { ...INIT_STATE },
            { disabled: disabled, autoClose: autoClose, options: options, showText: showText }
        )

        this.setState(state)
        console.log(state)
    }

    handlerClickSA() {
        console.log("isOpened:", this.state.isOpened)
        console.log(this.handlerClickSA.name)
    }

    handleronOpenedSA() {
        console.log("isOpened:", this.state.isOpened)
        console.log(this.handleronOpenedSA.name)
    }

    handleronClosedSA() {
        console.log("isOpened:", this.state.isOpened)
        console.log(this.handleronClosedSA.name)
    }

    handlerOnOpenedSingle(index) {
        const list = this.state.list.map((item, key) => {
            if (key === index) {
                item.isOpened = true
            } else {
                item.isOpened = false
            }
            return item
        })

        this.setState({ list })
    }

    render() {
        const { isOpened, disabled, autoClose, options, showText, list } = this.state

        console.log("list:", list, "options:", options)

        return (
            <View>
                <View>
                    <AtButton type='primary' size='normal'
                        onClick={this.handlerClickBtn.bind(this, false, false,
                            [{ text: '取消', style: { backgroundColor: '#6190E8' } }, { text: '确认', style: { backgroundColor: '#FF4949' } }], '一般使用场景')}
                    >一般使用场景</AtButton>
                </View>

                <View>
                    <AtButton type='primary' size='normal'
                        onClick={this.handlerClickBtn.bind(this, true, false,
                            [{ text: '取消', style: { backgroundColor: '#6190E8' } }, { text: '确认', style: { backgroundColor: '#FF4949' } }], '禁止滑动展示')}
                    >禁止滑动展示</AtButton>
                </View>

                <View>
                    <AtButton type='primary' size='normal'
                        onClick={this.handlerClickBtn.bind(this, false, true,
                            [{ text: '取消', style: { backgroundColor: '#6190E8' } }, { text: '确认', style: { backgroundColor: '#FF4949' } }], '自动关闭')}
                    >自动关闭</AtButton>
                </View>
                <View>
                    <AtSwipeAction
                        isOpened={isOpened}
                        disabled={disabled}
                        autoClose={autoClose}
                        options={options}
                        onClick={this.handlerClickSA.bind(this)}
                        onClosed={this.handleronClosedSA.bind(this)}
                        onOpened={this.handleronOpenedSA.bind(this)}
                    >
                        <View>{showText}</View>
                    </AtSwipeAction>
                </View>

                <View> 与LIST一起使用 </View>
                <View>
                    <AtList>
                        <AtSwipeAction options={[
                            {
                                text: '取消',
                                style: {
                                    backgroundColor: '#6190E8'
                                }
                            },
                            {
                                text: '确认',
                                style: {
                                    backgroundColor: '#FF4949'
                                }
                            }
                        ]}>
                            <AtListItem title='Item1' />
                        </AtSwipeAction>
                        <AtSwipeAction options={[
                            {
                                text: '取消',
                                style: {
                                    backgroundColor: '#6190E8'
                                }
                            },
                            {
                                text: '确认',
                                style: {
                                    backgroundColor: '#FF4949'
                                }
                            }
                        ]}>
                            <AtListItem title='Item2' />
                        </AtSwipeAction>
                        <AtSwipeAction
                            options={[
                                {
                                    text: '警告',
                                    style: {
                                        backgroundColor: '#FFC82C'
                                    }
                                }
                            ]}
                        >
                            <AtListItem title='Item3' />
                        </AtSwipeAction>
                    </AtList>
                </View>

                <View>
                    <AtList>
                        {list.map((item, index) => (
                            <AtSwipeAction
                                isOpened={item.isOpened}
                                autoClose={true}
                                index={index}
                                options={options}
                                onOpened={this.handlerOnOpenedSingle.bind(this, index)}
                            >
                                <AtListItem title={item.title} />
                            </AtSwipeAction>
                        ))}
                    </AtList>
                </View>

            </View >
        )
    }
}