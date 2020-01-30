
/* 使用了从官方taroui拷贝过来的style文件夹的scss，
如果没有此文件夹， 则显示不生效 */

import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import '../../style/components/flex.scss'
import './flex.scss'

export default class AtViewCSS extends Component {
    constructor() {
        super(...arguments)
    }

    render() {
        return (
            <View>
                <View>一般用法</View>
                <View className='at-row'>
                    <View className='at-col'>A</View>
                    <View className='at-col'>B</View>
                    <View className='at-col'>C</View>
                </View>
                <View> 使用栅格化（长度）</View>
                <View className='at-row'>
                    <View className='at-col at-col-3'>A</View>
                    <View className='at-col at-col-6'>B</View>
                    <View className='at-col at-col-2'>C</View>
                    <View className='at-col at-col-1'>D</View>
                </View>
                <View>使用栅格（偏移）</View>
                <View className='at-row'>
                    <View className='at-col at-col__offset-2'>A</View>
                    <View className='at-col at-col__offset-3'>B</View>
                    <View className='at-col'>C</View>
                </View>
                <View>使用栅格（超出换行）</View>
                <View className='at-row at-row--wrap'>
                    <View className='at-col at-col-4'>A</View>
                    <View className='at-col at-col-4'>B</View>
                    <View className='at-col at-col-4'>C</View>
                    <View className='at-col at-col-4'>D</View>
                    <View className='at-col at-col-4'>E</View>
                </View>
                <View>宽度根据内容撑开</View>
                <View className='at-row'>
                    <View className='at-col at-col-1 at-col--auto'> 被内容撑开 </View>
                    <View className='at-col'>B</View>
                </View>
                <View>侧轴方向的对齐方式</View>
                <View className='at-row'>
                    <View style='height:100px' className='at-col'>A</View>
                    <View className='at-col'>默认对齐方式 -- stretch</View>
                </View>
                <View className='at-row at-row__align--start'>
                    <View style='height:100px' className='at-col'>B</View>
                    <View className='at-col'>顶部对齐 -- start</View>
                </View>
                <View className='at-row at-row__align--center'>
                    <View style='height:100px' className='at-col'>C</View>
                    <View className='at-col'>居中对齐 -- center</View>
                </View>
                <View className='at-row at-row__align--end'>
                    <View style='height:100px' className='at-col'>D</View>
                    <View className='at-col'>底部对齐 -- end</View>
                </View>
                <View>主轴方向的排列方式</View>
                <View className='at-row'>
                    <View className='at-col at-col-5'>默认</View>
                    <View className='at-col at-col-5'>Start</View>
                </View>
                <View className='at-row at-row__justify--end'>
                    <View className='at-col at-col-5'>底部排列</View>
                    <View className='at-col at-col-5'>End</View>
                </View>
                <View className='at-row at-row__justify--center'>
                    <View className='at-col at-col-5'>居中排列</View>
                    <View className='at-col at-col-5'>Center</View>
                </View>
                <View className='at-row at-row__justify--between'>
                    <View className='at-col at-col-5'>左右排列</View>
                    <View className='at-col at-col-5'>Between</View>
                </View>
                <View className='at-row at-row__justify--around'>
                    <View className='at-col at-col-5'>平均排列</View>
                    <View className='at-col at-col-5'>Around</View>
                </View>
            </View>
        )
    }
}