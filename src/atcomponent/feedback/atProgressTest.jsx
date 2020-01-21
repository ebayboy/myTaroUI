import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtProgress, AtButton } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

//import './AtProgress.scss';

export default class AtProgressTest extends Component {
    constructor() {
        super(...arguments)
        this.state = {
            color: 'red',
            status: 'progress', //progress,error,success
            percent: 25,
            strokeWidth: 6, // 6, 8, 10
            isHidePercent: false
        }
    }

    handleClick(type) {
        const step = 5
        let mystatus = 'progress'
        let mypercent = this.state.percent

        if (type === 'add') {
            if (mypercent < 100) {
                mypercent = mypercent + step
            }
        } else {
            if (mypercent > 0) {
                mypercent = mypercent - step
            }
        }

        console.log("handleClick set type:%s status:%s, percent:%d", type, mystatus, mypercent)

        if (mypercent === 100) {
            mystatus = 'success'
        } else if (mypercent===0) {
            mystatus = 'error'
        } else {
            mystatus = 'progress'
        }
        
        this.setState({
            status: mystatus,
            percent: mypercent
        })
    }

    render() {
        return (
            <View>
                <AtProgress color={this.state.color}
                    status={this.state.status}
                    percent={this.state.percent}
                    strokeWidth={this.state.strokeWidth}
                    isHidePercent={this.state.isHidePercent}
                >
                </AtProgress>
                <AtButton type='primary' color='red' onClick={this.handleClick.bind(this, 'sub')}>error</AtButton>
                <AtButton type='primary' color='green' onClick={this.handleClick.bind(this, 'add')}>success</AtButton>
            </View>
        )
    }
}