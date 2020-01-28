import Taro, { Component } from '@tarojs/taro'
import { View, Picker } from '@tarojs/components'

import { AtForm } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

const INIT_STATE = {
    mode: 'selector',
    range: ['中国', '日本', '美国', '韩国', '其他'],
    selectorChecked: '中国',
    timeSel: '12:01',
    dateSel: '2020-01-28',
    multiRange: [['稀饭', '牛奶', '咖啡'], ['猪肉', '牛肉', '鸡肉']],
    multiSel: [0, 1]
}
export default class PickerTest extends Component {
    constructor() {
        super(...arguments)

        this.state = { ...INIT_STATE }
    }

    handleChange(event) {
        console.log("event.detail.value:", event.detail.value)
        this.setState({ selectorChecked: this.state.range[event.detail.value] })
    }

    onTimeChange(event) {
        console.log("event:", event)
        this.setState({ timeSel: event.detail.value })
    }
    onDateChange(e) {
        console.log("e:", e)
        this.setState({ dateSel: e.detail.value })
    }

    handleMultiSel(e) {
        this.setState({ multiSel: e.detail.value })
        console.log("e.detail.value:", e.detail.value)
        console.log("this.state.multiSel:", this.state.multiSel)
    }

    render() {
        return (
            <View className='container'>
                <View className='page-body'>
                    <View className='page-section'>
                        <Text>地区选择器</Text>
                        <View>
                            <Picker
                                mode='selector'
                                value={this.state.selectorChecked}
                                range={this.state.range}
                                onChange={this.handleChange.bind(this)}>
                                <View className='picker'>
                                    当前选择：{this.state.selectorChecked}
                                </View>
                            </Picker>
                        </View>
                    </View>
                    <View className='page-section'>
                        <Text>时间选择器</Text>
                        <View>
                            <Picker mode='time'
                                value={this.state.timeSel}
                                onChange={this.onTimeChange}>
                                <View className='picker'>
                                    当前选择：{this.state.timeSel}
                                </View>
                            </Picker>
                        </View>
                    </View>
                    <View className='page-section'>
                        <Text>日期选择器</Text>
                        <View>
                            <Picker mode='date'
                                value={this.state.dateSel}
                                onChange={this.onDateChange}>
                                <View className='picker'>
                                    当前选择：{this.state.dateSel}
                                </View>
                            </Picker>
                        </View>
                    </View>
                    <View className='page-section'>
                        <Text>多列选择器</Text>
                        <View>
                            <Picker
                                value={this.state.multiSel}
                                mode='multiSelector'
                                range={this.state.multiRange}
                                onChange={this.handleMultiSel.bind(this)}>
                                <View className='picker'>
                                    请选择早餐： {`${this.state.multiRange[0][this.state.multiSel[0]]} & ${this.state.multiRange[1][this.state.multiSel[1]]}`}
                                </View>
                            </Picker>
                        </View>
                    </View>
                </View>
            </View>
        )
    }
}